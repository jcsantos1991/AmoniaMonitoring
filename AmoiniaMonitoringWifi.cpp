#include "AmoiniaMonitoringWifi.h"

#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
#define MQTT_NAME "JoseCarlos"
#define MQTT_PASS "aio_eOTf38jrFx4MYUTnK6wtEzdE1F5r"

#define JOSE 0
#define ERIC 1
#define USER ERIC
#define HOSTIFTTT "maker.ifttt.com"

#if USER == JOSE
#define EVENT "update_file"
#define IFTTTKEY "ikEmNGUyfLBmgygDv4zeIP7XmFfvq0WumlC9YekwOyI"
#else
#define EVENT "loadData"
#define IFTTTKEY "boO4wJf-IuhT4W3VnDL6lS"
#endif

AmoiniaMonitoringWifi::AmoiniaMonitoringWifi() : mqtt(&clientMQTT, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS),
                                                 onoff(&mqtt, MQTT_NAME "/f/onoff"),
                                                 currentheater1(&mqtt, MQTT_NAME "/f/currentheater1"),
                                                 currentheater2(&mqtt, MQTT_NAME "/f/currentheater2"),
                                                 gainsensor1(&mqtt, MQTT_NAME "/f/gainsensor1"),
                                                 gainsensor2(&mqtt, MQTT_NAME "/f/gainsensor2"),
                                                 r1sensor1(&mqtt, MQTT_NAME "/f/r1sensor1"),
                                                 r1sensor2(&mqtt, MQTT_NAME "/f/r1sensor2"),
                                                 vdacsensor1(&mqtt, MQTT_NAME "/f/vdacsensor1"),
                                                 vdacsensor2(&mqtt, MQTT_NAME "/f/vdacsensor2"),
                                                 setupControl(&mqtt, MQTT_NAME "/f/Setup")
{
    mqtt.subscribe(&onoff);
    mqtt.subscribe(&currentheater1);
    mqtt.subscribe(&gainsensor1);
    mqtt.subscribe(&r1sensor1);
    mqtt.subscribe(&vdacsensor1);
    mqtt.subscribe(&currentheater2);
    mqtt.subscribe(&gainsensor2);
    mqtt.subscribe(&r1sensor2);
    mqtt.subscribe(&vdacsensor2);
    mqtt.subscribe(&setupControl);
}

AmoiniaMonitoringWifi::~AmoiniaMonitoringWifi()
{
}

bool AmoiniaMonitoringWifi::connectAmoiniaMonitoringWifi(String ssidSetup, String passSetup, int maxTries)
{
    int numberOfTries = 0;
    statusWIFI = WL_IDLE_STATUS;
    ssid = ssidSetup; // your network SSID (name)
    pass = passSetup; // your network password (use for WPA, or use as key for WEP)

    // check for the presence of the shield:
    if (WiFi.status() == WL_NO_SHIELD)
    {
        Serial.println("WiFi shield not present");
        // don't continue:
        return false;
    }
    // attempt to connect to WiFi network:
    while ((statusWIFI != WL_CONNECTED) && (numberOfTries < maxTries))
    {
        Serial.print("Attempting to connect to SSID:");
        Serial.println(ssid);
        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        statusWIFI = WiFi.begin(ssid, pass);
        // wait 10 seconds for connection:
        delay(10000);
        numberOfTries++;
    }
    if (statusWIFI == WL_CONNECTED)
    {
        Serial.println("Connected to wifi");
        // print the SSID of the network you're attached to:
        Serial.print("SSID: ");
        Serial.println(WiFi.SSID());
        // print your WiFi shield's IP address:
        IPAddress ip = WiFi.localIP();
        Serial.print("IP Address: ");
        Serial.println(ip);
        // print the received signal strength:
        long rssi = WiFi.RSSI();
        Serial.print("signal strength (RSSI):");
        Serial.print(rssi);
        Serial.println(" dBm");
        return true;
    }
    else
    {
        Serial.println(" No Connected to wifi");
        return false;
    }
}

void AmoiniaMonitoringWifi::setFolder(String dFolder)
{
    folder = dFolder;
}

void AmoiniaMonitoringWifi::connectMQTT()
{
    int ret;
    if (mqtt.connected())
    {
        return;
    }
    Serial.print("Connecting to MQTT... ");

    if ((ret = mqtt.connect()) != 0) // connect will return 0 for connected
    {
        Serial.println(mqtt.connectErrorString(ret));
        mqtt.disconnect();
    }
    else
    {
        Serial.println("MQTT Connected!");
    }
}

void AmoiniaMonitoringWifi::readSubscriptionMQTT(SetupSystem &setup1, SetupSystem &setup2, bool &fan, int &status)
{
    Adafruit_MQTT_Subscribe *subscription;
    String str;
    float value;
    int valueB;
    status = 0;
    while ((subscription = mqtt.readSubscription(5000)))
    {
        // If we're in here, a subscription updated...
        if (subscription == &onoff)
        {
            Serial.print("onoff: ");
            Serial.println((char *)onoff.lastread);
            if (!strcmp((char *)onoff.lastread, "ON"))
            {
                status = 1;
            }
            else
            {
                status = 2;
            }
        }
        else if (subscription == &currentheater1)
        {
            Serial.print("currentheater1: ");
            str = (char *)currentheater1.lastread;
            value = str.toFloat();
            Serial.println(value, 3);
            setup1.currentHeater = value;
            status = 3;
        }
        else if (subscription == &currentheater2)
        {
            Serial.print("currentheater2: ");
            str = (char *)currentheater2.lastread;
            value = str.toFloat();
            Serial.println(value);
            setup2.currentHeater = value;
            status = 4;
        }
        else if (subscription == &gainsensor1)
        {
            // Print the new value to the serial monitor
            Serial.print("gainsensor1: ");
            str = (char *)gainsensor1.lastread;
            value = str.toFloat();
            Serial.println(value);
            setup1.rg = value;
            status = 3;
        }
        else if (subscription == &gainsensor2)
        {
            Serial.print("gainsensor2: ");
            str = (char *)gainsensor2.lastread;
            value = str.toFloat();
            Serial.println(value);
            setup2.rg = value;
            status = 4;
        }
        else if (subscription == &r1sensor1)
        {
            Serial.print("r1sensor1: ");
            str = (char *)r1sensor1.lastread;
            value = str.toFloat();
            Serial.println(value);
            setup1.r1 = value;
            status = 3;
        }
        else if (subscription == &r1sensor2)
        {
            Serial.print("r1sensor2: ");
            str = (char *)r1sensor2.lastread;
            value = str.toFloat();
            Serial.println(value);
            setup2.r1 = value;
            status = 4;
        }
        else if (subscription == &vdacsensor1)
        {
            Serial.print("vdacsensor1: ");
            str = (char *)vdacsensor1.lastread;
            value = str.toFloat();
            Serial.println(value);
            setup1.vDac = value;
            status = 3;
        }
        else if (subscription == &vdacsensor2)
        {
            Serial.print("vdacsensor2: ");
            str = (char *)vdacsensor2.lastread;
            value = str.toFloat();
            Serial.println(value);
            setup2.vDac = value;
            status = 4;
        }
        else if (subscription == &setupControl)
        {

            Serial.print("Setup: ");
            str = (char *)setupControl.lastread;
            valueB = str.toInt();
            Serial.println(valueB);
            if (bitRead(valueB, 3) == 1)
            {
                fan = true;
            }
            else
            {
                fan = false;
            }
            if (bitRead(valueB, 2) == 0)
            {
                if (bitRead(valueB, 1) == 1)
                {
                    setup1.aControlHeater = true;
                }
                else
                {
                    setup1.aControlHeater = false;
                }
                if (bitRead(valueB, 0) == 1)
                {
                    setup1.aControlPara = true;
                }
                else
                {
                    setup1.aControlPara = false;
                }
                status = 3;
            }
            else
            {
                if (bitRead(valueB, 1) == 1)
                {
                    setup2.aControlHeater = true;
                }
                else
                {
                    setup2.aControlHeater = false;
                }
                if (bitRead(valueB, 0) == 1)
                {
                    setup2.aControlPara = true;
                }
                else
                {
                    setup2.aControlPara = false;
                }
                status = 4;
            }
        }
    }
    // if (!mqtt.ping())
    // {
    //     mqtt.disconnect();
    //     Serial.println("MQTT Disconnected!");
    // }
}

bool AmoiniaMonitoringWifi::updateClockWifi(RTCZero &clock)
{
    unsigned long epoch;
    time_t utc;
    TimeChangeRule esCET = {"CET", Last, Sun, Oct, 3, 60};   // UTC + 1 hours
    TimeChangeRule esCEST = {"CEST", Last, Sun, Mar, 2, 120 }; // UTC + 2 hours
    Timezone spainTZ(esCET, esCEST);
    int numberOfTries = 0, maxTries = 6;
    do
    {
        epoch = WiFi.getTime();
        numberOfTries++;
    } while ((epoch == 0) && (numberOfTries < maxTries));
    if (numberOfTries == maxTries)
    {
        Serial.println("NTP unreachable!!");
        return false;
    }
    else
    {
        utc = epoch;
        time_t local = spainTZ.toLocal(utc);
        clock.setDate(day(local), month(local), year(local) - 2000);
        clock.setTime(hour(local), minute(local), second(local));
        return true;
    }
}

bool AmoiniaMonitoringWifi::triggerAnEvent(String value1, String value2, String value3, String event, String key)
{
    //  Construimos la petición HTTP
    client.println(String("GET ") + "/trigger/" + String(event) + "/with/key/" + String(key) + "?value1=" + value1 + "&value2=" + value2 + "&value3=" + value3 + " HTTP/1.1");
    client.println("Host: " + String(HOSTIFTTT));
    client.println("Connection: close");
    client.println();
    return true;
}

bool AmoiniaMonitoringWifi::triggerAnEvent(String value1, float value2, float value3, String event, String key)
{
    //  Construimos la petición HTTP
    client.println(String("GET ") + "/trigger/" + String(event) + "/with/key/" + String(key) + "?value1=" + value1 + "&value2=" + value2 + "&value3=" + value3 + " HTTP/1.1");
    client.println("Host: " + String(HOSTIFTTT));
    client.println("Connection: close");
    client.println();
    // Serial.println(value2);
    // Serial.println("Data Upload");
    return true;
}

bool AmoiniaMonitoringWifi::uploadMesasurentWifi(String filename, String data)
{
    bool lost = false;
    bool upload = true;
    int countData = 0;
    unsigned long pos = 0;
    String cal;
    String sensor1_2;
    String sensorT_H;
    float value1;
    float value2;
    float value3;
    float value4;
    String filenameLost;
    String dataLost;
    if (isConnectAmoiniaMonitoringWifi())
    {
        if (SD.exists("FileLost.txt"))
        {
            getPosFileLost(pos);
            File fileCVS = SD.open("FileLost.txt");
            if (fileCVS)
            {
                fileCVS.seek(pos);
                while (fileCVS.available() && (countData != 5))
                {
                    if (client.connect(HOSTIFTTT, 80))
                    {
                        Serial.println("Load Lost data");
                        filenameLost = (fileCVS.readStringUntil('\n'));
                        filenameLost.remove(filenameLost.length() - 1);
                        dataLost = (fileCVS.readStringUntil('\n'));
                        dataLost.remove(dataLost.length() - 1);
                        decodeData(dataLost, cal);
                        triggerAnEvent(filenameLost.substring(0, filenameLost.length() - 4), folder, cal, EVENT, IFTTTKEY);
                        while (client.connected())
                        {
                            if (client.available())
                            {
                                // read an incoming byte from the server and print it to serial monitor:
                                char c = client.read();
                                Serial.print(c);
                            }
                        }
                        // the server's disconnected, stop the client:
                        delay(1000);
                        client.stop();
                        // Serial.println("Server Connection Closed!!");
                        pos = fileCVS.position();
                        setPosFileLost(pos);
                        countData++;
                        //  Serial.println("Lost data");
                        //  Serial.println(dataLost);
                    }
                    else
                    {
                        Serial.println("Server Connection failed!!");
                        break;
                    }
                }
                if (fileCVS.available())
                {
                    upload = false;
                    lost = true;
                    fileCVS.close();
                }
                else
                {
                    upload = true;
                    lost = false;
                    fileCVS.close();
                    pos = 0;
                    setPosFileLost(pos);
                    SD.remove("FileLost.txt");
                }
            }
            else
            {
                Serial.println("Error File open...");
            }
        }
        if (upload == true)
        {
            if (client.connect(HOSTIFTTT, 80))
            {
                // decodeData(data, cal, value1, value2, value3, value4);
                // triggerAnEvent(cal, value1, value3, "Graph", IFTTTKEY);
                decodeData(data, cal);
                triggerAnEvent(filename.substring(0, filename.length() - 4), folder, cal, EVENT, IFTTTKEY);
                while (client.connected())
                {
                    if (client.available())
                    {
                        // read an incoming byte from the server and print it to serial monitor:
                        char c = client.read();
                        Serial.print(c);
                    }
                }
                // the server's disconnected, stop the client:
                client.stop();
                // Serial.println("Server Connection Closed!!");
            }
            else
            {
                Serial.println("Server Connection failed!!");
                lost = true;
            }
        }
    }
    else
    {
        lost = true;
    }
    if (lost == true)
    {
        uploadFileLost(filename, data);
        return false;
    }
    return true;
}

bool AmoiniaMonitoringWifi::uploadFileLost(String filename, String data)
{
    File fileError = SD.open("FileLost.txt", FILE_WRITE);
    if (fileError)
    {
        fileError.println(filename);
        fileError.println(data);
        fileError.close();
        return true;
    }
    Serial.println("Error File open...");
    return false;
}

bool AmoiniaMonitoringWifi::setPosFileLost(unsigned long pos)
{
    if (SD.exists("Setup.txt"))
    {
        SD.remove("Setup.txt");
    }
    File setupFileLost = SD.open("Setup.txt", FILE_WRITE);
    if (setupFileLost)
    {
        // Serial.print("setPosition");
        // Serial.println(pos);
        setupFileLost.seek(0);
        setupFileLost.println(pos);
        setupFileLost.close();
        return true;
    }
    Serial.println("Error File open...");
    return false;
}

bool AmoiniaMonitoringWifi::getPosFileLost(unsigned long &pos)
{
    String posStr;
    if (!SD.exists("Setup.txt"))
    {
        setPosFileLost(0);
    }
    File setupFileLost = SD.open("Setup.txt", FILE_READ);
    if (setupFileLost)
    {
        posStr = (setupFileLost.readStringUntil('\n'));
        posStr.remove(posStr.length() - 1);
        pos = posStr.toInt();
        // Serial.print("getPosition");
        // Serial.println(pos);
        setupFileLost.close();
        return true;
    }
    Serial.println("Error File open...");
    return false;
}

bool AmoiniaMonitoringWifi::isConnectAmoiniaMonitoringWifi()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        return true;
    }
    return false;
}

void AmoiniaMonitoringWifi::decodeData(String data, String &calendar, float &valueSensor1, float &valueSensor2, float &valueTemp, float &valueHumidity)
{
    String temp;
    int index1 = 0;
    int index2;
    int size = data.length();
    index2 = data.indexOf(",", index1);
    calendar = data.substring(index1, index2);
    index1 = index2 + 1;
    index2 = data.indexOf(",", index1);
    calendar = calendar + "|||" + data.substring(index1, index2);
    index1 = index2 + 1;
    index2 = data.indexOf(",", index1);
    valueSensor1 = (data.substring(index1, index2)).toFloat();
    index1 = index2 + 1;
    index2 = data.indexOf(",", index1);
    valueSensor2 = (data.substring(index1, index2)).toFloat();
    index1 = index2 + 1;
    index2 = data.indexOf(",", index1);
    valueTemp = (data.substring(index1, index2)).toFloat();
    index1 = index2 + 1;
    valueHumidity = (data.substring(index1, size)).toFloat();
}

void AmoiniaMonitoringWifi::decodeData(String data, String &calendar, String &valueSensor1_2, String &valueTempHumidity)
{
    String temp;
    int index1 = 0;
    int index2;
    int size = data.length();
    index2 = data.indexOf(",", index1);
    calendar = data.substring(index1, index2);
    index1 = index2 + 1;
    index2 = data.indexOf(",", index1);
    calendar = calendar + "%20" + data.substring(index1, index2);
    index1 = index2 + 1;
    index2 = data.indexOf(",", index1);
    valueSensor1_2 = data.substring(index1, index2);
    index1 = index2 + 1;
    index2 = data.indexOf(",", index1);
    valueSensor1_2 = valueSensor1_2 + "|||" + (data.substring(index1, index2));
    index1 = index2 + 1;
    index2 = data.indexOf(",", index1);
    valueTempHumidity = (data.substring(index1, index2));
    index1 = index2 + 1;
    index2 = data.indexOf("<", index1);
    valueTempHumidity = valueTempHumidity + "|||" + (data.substring(index1, index2));
}

void AmoiniaMonitoringWifi::decodeData(String data, String &dataDecode)
{
    String temp;
    Serial.println(data);
    int index1 = 0;
    int index2;
    int size = data.length();
    index2 = data.indexOf(",", index1);
    dataDecode = data.substring(index1, index2);
    index1 = index2 + 1;
    index2 = data.indexOf(",", index1);
    dataDecode = dataDecode + "%20" + data.substring(index1, index2);
    index1 = index2 + 1;
    index2 = data.indexOf(",", index1);
    dataDecode = dataDecode + "|||" + data.substring(index1, index2); // 1
    index1 = index2 + 1;
    index2 = data.indexOf(",", index1);
    dataDecode = dataDecode + "|||" + (data.substring(index1, index2)); // 2
    index1 = index2 + 1;
    index2 = data.indexOf(",", index1);
    dataDecode = dataDecode + "|||" + (data.substring(index1, index2)); // 3
    index1 = index2 + 1;
    index2 = data.indexOf(",", index1);
    dataDecode = dataDecode + "|||" + (data.substring(index1, index2)); // 4
    index1 = index2 + 1;
    index2 = data.indexOf(",", index1);
    dataDecode = dataDecode + "|||" + data.substring(index1, index2); // 5
    index1 = index2 + 1;
    index2 = data.indexOf(",", index1);
    dataDecode = dataDecode + "|||" + (data.substring(index1, index2)); // 6
    index1 = index2 + 1;
    index2 = data.indexOf(",", index1);
    dataDecode = dataDecode + "|||" + (data.substring(index1, index2)); // 7
    index1 = index2 + 1;
    index2 = data.indexOf(",", index1);
    dataDecode = dataDecode + "|||" + (data.substring(index1, index2)); // 8
    index1 = index2 + 1;
    index2 = data.indexOf(",", index1);
    dataDecode = dataDecode + "|||" + data.substring(index1, index2); // 9
    index1 = index2 + 1;
    index2 = data.indexOf(",", index1);
    dataDecode = dataDecode + "|||" + (data.substring(index1, index2)); // 10
    index1 = index2 + 1;
    index2 = data.indexOf(",", index1);
    dataDecode = dataDecode + "|||" + (data.substring(index1, index2)); // 11
    index1 = index2 + 1;
    index2 = data.indexOf(",", index1);
    dataDecode = dataDecode + "|||" + (data.substring(index1, index2)); // 12
    index1 = index2 + 1;
    index2 = data.indexOf(",", index1);
    dataDecode = dataDecode + "|||" + data.substring(index1, index2); // 13
    index1 = index2 + 1;
    index2 = data.indexOf(",", index1);
    dataDecode = dataDecode + "|||" + (data.substring(index1, index2)); // 14
    index1 = index2 + 1;
    index2 = data.indexOf(",", index1);
    dataDecode = dataDecode + "|||" + (data.substring(index1, index2)); // 15
    index1 = index2 + 1;
    index2 = data.indexOf("<", index1);
    dataDecode = dataDecode + "|||" + (data.substring(index1, index2)); // 16
    Serial.println(dataDecode);
}
