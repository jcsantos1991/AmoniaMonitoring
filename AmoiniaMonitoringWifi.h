#ifndef AMOINIAMONITORINGWIFI_H
#define AMOINIAMONITORINGWIFI_H

#include "Arduino.h"
#include <SPI.h>
#include <WiFi101.h>
#include <RTCZero.h>
#include <SD.h>
#include <Timezone.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "AmoniaMonitoring.h"

class AmoiniaMonitoringWifi
{
public:
    AmoiniaMonitoringWifi();
    ~AmoiniaMonitoringWifi();
    bool connectAmoiniaMonitoringWifi(String ssidSetup, String passSetup, int maxTries);
    bool updateClockWifi(RTCZero &clock);
    bool uploadMesasurentWifi(String filename, String data);
    bool triggerAnEvent(String value1, String value2, String value3, String event, String key);
    bool triggerAnEvent(String value1, float value2, float value3, String event, String key);
    bool uploadFileLost(String filename, String data);
    void connectMQTT();
    void readSubscriptionMQTT(SetupSystem &setup1, SetupSystem &setup2, bool &fan, int &status);
    bool isConnectAmoiniaMonitoringWifi();
    bool setPosFileLost(unsigned long pos);
    void setFolder(String dFolder);
    bool getPosFileLost(unsigned long &pos);
    void decodeData(String data, String &calendar, float &valueSensor1, float &valueSensor2, float &valueTemp, float &valueHumidity);
    void decodeData(String data, String &calendar, String &valueSensor1_2, String &valueTempHumidity);
    void decodeData(String data, String &dataDecode);

private:
    int statusWIFI;
    WiFiClient client;
    WiFiClient clientMQTT;
    Adafruit_MQTT_Client mqtt;
    Adafruit_MQTT_Subscribe onoff;
    Adafruit_MQTT_Subscribe currentheater1;
    Adafruit_MQTT_Subscribe currentheater2;
    Adafruit_MQTT_Subscribe gainsensor1;
    Adafruit_MQTT_Subscribe gainsensor2;
    Adafruit_MQTT_Subscribe r1sensor1;
    Adafruit_MQTT_Subscribe r1sensor2;
    Adafruit_MQTT_Subscribe vdacsensor1;
    Adafruit_MQTT_Subscribe vdacsensor2;
    Adafruit_MQTT_Subscribe setupControl;
    String ssid;
    String pass;
    String folder;
};

#endif