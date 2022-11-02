#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <SD.h>
#include <RTCZero.h>
#include <Wire.h>
#include <SHTSensor.h>
#include "AmmoniaSensor.h"
#include "StoreMeasurements.h"
#include "AmoiniaMonitoringWifi.h"
#include "TempHumiSensor.h"
#include "HeaterControl.h"
#include "FanControl.h"

#define PERIOD 2500
#define N_PERIOD 12
#define N_DATA 5
#define N_SENSORS 16
#define R1_CH1 30000.0
#define R1_CH2 30000.0
#define R2_CH1 1000.0
#define R2_CH2 1000.0
#define R3_CH1 29880.0
#define R3_CH2 29880.0
#define RG_CH1 1000.0
#define RG_CH2 1000.0
#define VREF 3.331
#define VOFFSET 1.5
#define HOUR_ALARM 0
#define MIN_ALARM 0
#define SEC_ALARM 0
#define PIN_CONTROL1_HEATER1 2
#define PIN_CONTROL2_HEATER1 1
#define PIN_CONTROL1_HEATER2 3
#define PIN_CONTROL2_HEATER2 5
#define VCC_CONTROL_HEATER 5.0
#define PIN_CONTROL_FAN 7
#define PIN_CS_SD 4
#define SET_CURRENT_HEATER1 0.100
#define SET_CURRENT_HEATER2 0.100
#define I2C_ADDR_2C 0x2C
#define I2C_ADDR_2D 0x2D
#define I2C_ADDR_2E 0x2E
#define I2C_ADDR_2F 0x2F

StoreMeasurements store;
AmmoniaSensor sensor1(I2C_ADDR_2C, I2C_ADDR_2D);
AmmoniaSensor sensor2(I2C_ADDR_2E, I2C_ADDR_2F);
HeaterControl heaterSensor1(INA219::I2C_ADDR_40);
HeaterControl heaterSensor2(INA219::I2C_ADDR_41);
RTCZero rtc;
String currentFileSD;
String currentFileOneDrive;
TempHumiSensor sensor3_4;
FanControl fan;
AmoiniaMonitoringWifi nodeAmoiniaMonitoring;
float valuesSensors[N_SENSORS];
String dataStr;
String dataStrTest;
int countData = 0;
bool enableSampleSensor = true;
bool enableSetCurrentHeater = true;
bool updateRTC = false;
bool setupC = false;
unsigned long txMillis = 0;
unsigned long testMillis = 0;
int m = 0;
int statusMqtt = 0;
bool stateFan = true;
SetupSystem setupSensor1 = {R1_CH1, RG_CH1, VOFFSET, 0.100, true, true};
SetupSystem setupSensor2 = {R1_CH2, RG_CH2, VOFFSET, 0.100, true, true};
String ssid = "yLAPTOP-CBH6O4RO 3235"; // your network SSID (name)
String pass = "y!0nX9057";             // your network password (use for WPA, or use as key for WEP)
String folder = "test";

void setup()
{
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  };
  Serial.println("Setting microSD...");
  store.setupStoreMeasurements(PIN_CS_SD);
  loadSetup();
  Serial.println("Setting channel Sensor 1...");
  sensor1.setupSensor(A1, setupSensor1.vDac, setupSensor1.r1, R2_CH1, setupSensor1.rg, VREF);
  Serial.println("Setting channel Sensor 2...");
  sensor2.setupSensor(A2, setupSensor2.vDac, setupSensor2.r1, R2_CH2, setupSensor2.rg, VREF);
  Serial.println("Setting heater Sensor 1...");
  heaterSensor1.setupHeaterControl(PIN_CONTROL1_HEATER1, PIN_CONTROL2_HEATER1);
  Serial.println("Setting heater Sensor 2...");
  heaterSensor2.setupHeaterControl(PIN_CONTROL1_HEATER2, PIN_CONTROL2_HEATER2);
  fan.setupFanControl(PIN_CONTROL_FAN);
  fan.setOnOffFan(stateFan);
  Serial.println("Setting Sensor Temperature-Humidity...");
  sensor3_4.setupSensor();
  rtc.begin();
  rtc.setTime(7, 0, 0);
  rtc.setDate(1, 8, 22);

  if (nodeAmoiniaMonitoring.connectAmoiniaMonitoringWifi(ssid, pass, 3))
  {
    if (nodeAmoiniaMonitoring.updateClockWifi(rtc))
    {
      updateRTC = true;
    }
  }
  nodeAmoiniaMonitoring.setFolder(folder);
  rtc.setAlarmTime(HOUR_ALARM, MIN_ALARM, SEC_ALARM);
  rtc.enableAlarm(rtc.MATCH_HHMMSS);
  rtc.attachInterrupt(alarmMatch);
  SdFile::dateTimeCallback(dateTime);
  store.newFileMeasurements(currentFileSD, rtc);
  Serial.println(currentFileSD);
  int count = 0;
  bool controlOK = false;
  setupTemperatureHeater1();
  setupTemperatureHeater2();
}

void loop()
{
  fan.setOnOffFan(stateFan);
  setupSytemMQTT();
  setupSystem();
  if (setupC == false)
  {
    if (enableSampleSensor)
    {
      enableSampleSensor = false;
      testMillis = millis();
      for (int i = 1; i < N_PERIOD; i++)
      {
        Serial.print("Current Heater1 = ");
        Serial.println(heaterSensor1.getCurrentHeater(), 3);
        if (setupSensor1.aControlHeater)
        {
          heaterSensor1.setCurrentHeater(setupSensor1.currentHeater, 0);
        }
        Serial.print("Voltage Heater 1 = ");
        Serial.println(heaterSensor1.getVoltageHeater(), 2);

        Serial.print("Current Heater2 = ");
        Serial.println(heaterSensor2.getCurrentHeater(), 3);
        if (setupSensor2.aControlHeater)
        {
          heaterSensor2.setCurrentHeater(setupSensor2.currentHeater, 0);
        }
        Serial.print("Voltage Heater 2 = ");
        Serial.println(heaterSensor2.getVoltageHeater(), 2);

        Serial.print("Sensor1 Resistence = ");
        Serial.println(sensor1.getSensorResistence() / 1000, 3);
        // sensor1.getSensorResistence();
        sensor1.meanSensorResistence(i);
        Serial.print("Sensor2 Resistence = ");
        Serial.println(sensor2.getSensorResistence() / 1000, 3);
        // sensor2.getSensorResistence();
        sensor2.meanSensorResistence(i);
        Serial.print(".");
        delay(PERIOD);
      }
      Serial.print("Time = ");
      Serial.println(millis() - txMillis);
    }
    if ((millis() - txMillis >= 57000) && (enableSampleSensor == false))
    {
      enableSampleSensor = true;

      sensor3_4.readSampleSensor();
      Serial.println();
      valuesSensors[0] = sensor1.getMeanSensorResistence() / 1000.0;
      valuesSensors[1] = sensor2.getMeanSensorResistence() / 1000.0;
      valuesSensors[2] = sensor3_4.getTempSensor();
      valuesSensors[3] = sensor3_4.getHumiditySensor();
      valuesSensors[4] = heaterSensor1.getCurrentHeater();
      valuesSensors[5] = heaterSensor2.getCurrentHeater();
      valuesSensors[6] = heaterSensor1.getVoltageHeater();
      valuesSensors[7] = heaterSensor2.getVoltageHeater();
      valuesSensors[8] = setupSensor1.currentHeater;
      valuesSensors[9] = setupSensor2.currentHeater;
      valuesSensors[10] = setupSensor1.r1 / 1000.0;
      valuesSensors[11] = setupSensor2.r1 / 1000.0;
      valuesSensors[12] = setupSensor1.rg / 1000.0;
      valuesSensors[13] = setupSensor2.rg / 1000.0;
      valuesSensors[14] = setupSensor1.vDac;
      valuesSensors[15] = setupSensor2.vDac;

      if (setupSensor1.aControlPara)
      {
        if (sensor1.calculateR1(sensor1.getMeanSensorResistence()))
        {
          setupSensor1.r1 = sensor1.getR1();
          saveSetupSensor1();
        }
      }
      if (setupSensor2.aControlPara)
      {
        if (sensor2.calculateR1(sensor2.getMeanSensorResistence()))
        {
          setupSensor2.r1 = sensor2.getR1();
          saveSetupSensor2();
        }
      }

      // Serial.print("Sensor1 Resistence Mean = ");
      // Serial.println(valuesSensors[0]);
      // Serial.print("Sensor2 Resistence Mean = ");
      // Serial.println(valuesSensors[1]);
      // Serial.print("Sensor3 Temperature = ");
      // Serial.println(valuesSensors[2], 2);
      // Serial.print("Sensor4 Humidity = ");
      // Serial.println(valuesSensors[3], 2);
      // Serial.print("Current Heater 1 = ");
      // Serial.println(valuesSensors[4], 3);
      // Serial.print("Current Heater 2 = ");
      // Serial.println(valuesSensors[5], 3);
      // Serial.print("Voltage Heater 1 = ");
      // Serial.println(valuesSensors[6], 2);
      // Serial.print("Voltage Heater 2 = ");
      // Serial.println(valuesSensors[7], 2);
      Serial.print("Time = ");
      Serial.println((millis() - txMillis) / 1000);
      txMillis = millis();
      store.addMeasurement(currentFileSD, rtc, valuesSensors, N_SENSORS, dataStrTest);
      if (!(nodeAmoiniaMonitoring.isConnectAmoiniaMonitoringWifi()))
      {
        if (nodeAmoiniaMonitoring.connectAmoiniaMonitoringWifi(ssid, pass, 1))
        {
          updateRTC = false;
        }
      }
      nodeAmoiniaMonitoring.uploadMesasurentWifi(currentFileSD, dataStrTest + "<br/>");
      if ((updateRTC == false) && (nodeAmoiniaMonitoring.isConnectAmoiniaMonitoringWifi()))
      {
        if (nodeAmoiniaMonitoring.updateClockWifi(rtc))
        {
          store.newFileMeasurements(currentFileSD, rtc);
          updateRTC = true;
        }
      }
      countData++;
      m++;
      Serial.print("\nSample #");
      Serial.println(m);
      if (countData == N_DATA)
      {
        countData = 0;
      }
    }
  }
}

void alarmMatch()
{
  Serial.println("ALARMA");
  store.newFileMeasurements(currentFileSD, rtc);
}

void loadSetup()
{
  String str;
  File fileSetup = SD.open("WiFiS.txt", FILE_READ);
  if (fileSetup)
  {
    ssid = (fileSetup.readStringUntil('\n'));
    ssid.remove(ssid.length() - 1);
    pass = (fileSetup.readStringUntil('\n'));
    pass.remove(pass.length() - 1);
    folder = (fileSetup.readStringUntil('\n'));
    folder.remove(folder.length() - 1);
    fileSetup.close();
  }
  fileSetup = SD.open("SepCh1.txt", FILE_READ);
  if (fileSetup)
  {
    str = (fileSetup.readStringUntil('\n'));
    str.remove(str.length() - 1);
    setupSensor1.r1 = str.toFloat();
    str = (fileSetup.readStringUntil('\n'));
    str.remove(str.length() - 1);
    setupSensor1.rg = str.toFloat();
    str = (fileSetup.readStringUntil('\n'));
    str.remove(str.length() - 1);
    setupSensor1.vDac = str.toFloat();
    str = (fileSetup.readStringUntil('\n'));
    str.remove(str.length() - 1);
    setupSensor1.currentHeater = str.toFloat();
    str = (fileSetup.readStringUntil('\n'));
    str.remove(str.length() - 1);
    setupSensor1.aControlHeater = str.toInt();
    str = (fileSetup.readStringUntil('\n'));
    str.remove(str.length() - 1);
    setupSensor1.aControlPara = str.toInt();
    fileSetup.close();
  }
  fileSetup = SD.open("SepCh2.txt", FILE_READ);
  if (fileSetup)
  {
    str = (fileSetup.readStringUntil('\n'));
    str.remove(str.length() - 1);
    setupSensor2.r1 = str.toFloat();
    str = (fileSetup.readStringUntil('\n'));
    str.remove(str.length() - 1);
    setupSensor2.rg = str.toFloat();
    str = (fileSetup.readStringUntil('\n'));
    str.remove(str.length() - 1);
    setupSensor2.vDac = str.toFloat();
    str = (fileSetup.readStringUntil('\n'));
    str.remove(str.length() - 1);
    setupSensor2.currentHeater = str.toFloat();
    str = (fileSetup.readStringUntil('\n'));
    str.remove(str.length() - 1);
    setupSensor2.aControlHeater = str.toInt();
    str = (fileSetup.readStringUntil('\n'));
    str.remove(str.length() - 1);
    setupSensor2.aControlPara = str.toInt();
    fileSetup.close();
  }
}

void saveSetupSensor1()
{
  File fileSetup;
  SD.remove("SepCh1.txt");
  fileSetup = SD.open("SepCh1.txt", FILE_WRITE);
  if (fileSetup)
  {
    fileSetup.println(setupSensor1.r1, 2);
    fileSetup.println(setupSensor1.rg, 2);
    fileSetup.println(setupSensor1.vDac, 3);
    fileSetup.println(setupSensor1.currentHeater, 3);
    fileSetup.println(setupSensor1.aControlHeater);
    fileSetup.println(setupSensor1.aControlPara);
    fileSetup.close();
  }
  sensor1.setupSensor(A1, setupSensor1.vDac, setupSensor1.r1, R2_CH1, setupSensor1.rg, VREF);
  Serial.println("saveSetupSensor1");
}

void saveSetupSensor2()
{
  File fileSetup;
  SD.remove("SepCh2.txt");
  fileSetup = SD.open("SepCh2.txt", FILE_WRITE);
  if (fileSetup)
  {
    fileSetup.println(setupSensor2.r1, 2);
    fileSetup.println(setupSensor2.rg, 2);
    fileSetup.println(setupSensor2.vDac, 3);
    fileSetup.println(setupSensor2.currentHeater, 3);
    fileSetup.println(setupSensor2.aControlHeater);
    fileSetup.println(setupSensor2.aControlPara);
    fileSetup.close();
  }
  sensor2.setupSensor(A2, setupSensor2.vDac, setupSensor2.r1, R2_CH2, setupSensor2.rg, VREF);
  Serial.println("saveSetupSensor2");
}

void setupSystem()
{
  if (Serial.available() > 0)
  {
    String str;
    char ch;
    char var;
    char cmd = Serial.read();
    File fileSetup;
    Serial.println(cmd);
    switch (cmd)
    {
    case 'S':
      setupC = true;
      break;
    case 'F':
      setupC = false;
      break;
    case 'W':
      ssid = Serial.readString();
      Serial.println(ssid);
      break;
    case 'P':
      pass = Serial.readString();
      Serial.println(pass);
      SD.remove("WiFiS.txt");
      fileSetup = SD.open("WiFiS.txt", FILE_WRITE);
      if (fileSetup)
      {
        fileSetup.println(ssid);
        fileSetup.println(pass);
        fileSetup.println(folder);
        fileSetup.close();
      }
      nodeAmoiniaMonitoring.connectAmoiniaMonitoringWifi(ssid, pass, 1);
      break;
    case 'O':
      folder = Serial.readString();
      Serial.println(folder);
      SD.remove("WiFiS.txt");
      fileSetup = SD.open("WiFiS.txt", FILE_WRITE);
      if (fileSetup)
      {
        fileSetup.println(ssid);
        fileSetup.println(pass);
        fileSetup.println(folder);
        fileSetup.close();
      }
      nodeAmoiniaMonitoring.setFolder(folder);
      break;
    case 'C':
      ch = Serial.read();
      var = Serial.read();
      switch (var)
      {
      case 'R':
        if (ch == '1')
        {
          str = Serial.readString();
          setupSensor1.r1 = str.toFloat();
        }
        else if (ch == '2')
        {
          str = Serial.readString();
          setupSensor2.r1 = str.toFloat();
        }
        break;
      case 'G':
        if (ch == '1')
        {
          str = Serial.readString();
          setupSensor1.rg = str.toFloat();
        }
        else if (ch == '2')
        {
          str = Serial.readString();
          setupSensor2.rg = str.toFloat();
        }
        break;
      case 'D':
        if (ch == '1')
        {
          str = Serial.readString();
          setupSensor1.vDac = str.toFloat();
        }
        else if (ch == '2')
        {
          str = Serial.readString();
          setupSensor2.vDac = str.toFloat();
        }
        break;
      case 'H':
        if (ch == '1')
        {
          str = Serial.readString();
          setupSensor1.currentHeater = str.toFloat();
        }
        else if (ch == '2')
        {
          str = Serial.readString();
          setupSensor2.currentHeater = str.toFloat();
        }
        break;
      case 'A':
        if (ch == '1')
        {
          str = Serial.readString();
          setupSensor1.aControlHeater = str.toInt();
          setupTemperatureHeater1();
        }
        else if (ch == '2')
        {
          str = Serial.readString();
          setupSensor2.aControlHeater = str.toInt();
          setupTemperatureHeater2();
        }
        break;
      case 'V':
        if (ch == '1')
        {
          str = Serial.readString();
          setupSensor1.aControlPara = str.toInt();
        }
        else if (ch == '2')
        {
          str = Serial.readString();
          setupSensor2.aControlPara = str.toInt();
        }
        break;
      }
      if (ch == '1')
      {
        saveSetupSensor1();
      }
      else if (ch == '2')
      {
        saveSetupSensor2();
      }
      break;
    case 'V':
      stateFan = true;
      break;
    case 'A':
      stateFan = false;
      break;
    default:
      break;
    }
  }
}

void setupSytemMQTT()
{
  if ((nodeAmoiniaMonitoring.isConnectAmoiniaMonitoringWifi()))
  {
    nodeAmoiniaMonitoring.connectMQTT();
    nodeAmoiniaMonitoring.readSubscriptionMQTT(setupSensor1, setupSensor2, stateFan, statusMqtt);
    if (statusMqtt == 1)
    {
      setupC = true;
    }
    else if (statusMqtt == 2)
    {
      setupC = false;
    }
    else if (statusMqtt == 3)
    {
      setupTemperatureHeater1();
      saveSetupSensor1();
    }
    else if (statusMqtt == 4)
    {
      setupTemperatureHeater2();
      saveSetupSensor2();
    }
  }
}

void dateTime(uint16_t *date, uint16_t *time)
{
  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(rtc.getYear() + 2000, rtc.getMonth(), rtc.getDay());
  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());
}

String to2digits(int number)
{
  if (number < 10)
  {
    return ("0" + String(number));
  }
  else
  {
    return String(number);
  }
}

void setupTemperatureHeater1()
{
  int count = 0;
  if (setupSensor1.aControlHeater)
  {
    Serial.println("Setting temperature on heater 1...");
    if (setupSensor1.currentHeater == 0)
    {
      heaterSensor1.setPWMHeater(0);
    }
    else
    {
      heaterSensor1.setPWMHeater(10);
      heaterSensor1.resetControlHeater();
      while (count < 20)
      {
        delay(1000);
        Serial.print("Current Heater1 = ");
        Serial.println(heaterSensor1.getCurrentHeater(), 3);
        if (heaterSensor1.setCurrentHeater(setupSensor1.currentHeater, 1))
          break;
        Serial.print("Voltage Heater 1 = ");
        Serial.println(heaterSensor1.getVoltageHeater(), 2);
        Serial.println(count);
        count++;
      }
    }
  }
}

void setupTemperatureHeater2()
{
  int count = 0;
  if (setupSensor2.aControlHeater)
  {
    Serial.println("Setting temperature on heater 2...");
    if (setupSensor2.currentHeater == 0)
    {
      heaterSensor2.setPWMHeater(0);
    }
    else
    {
      heaterSensor2.setPWMHeater(10);
      heaterSensor2.resetControlHeater();
      while (count < 20)
      {
        delay(1000);
        Serial.print("Current Heater2 = ");
        Serial.println(heaterSensor2.getCurrentHeater(), 3);
        if (heaterSensor2.setCurrentHeater(setupSensor2.currentHeater, 1))
          break;
        Serial.print("Voltage Heater 2 = ");
        Serial.println(heaterSensor2.getVoltageHeater(), 2);
        Serial.println(count);
        count++;
      }
    }
  }
}