#line 1 "c:\\Users\\MinosPortatil2\\OneDrive - URV\\Amonnia Monitoring\\AmoniaMonitoring\\HeaterControl.h"
#ifndef HEATERCONTROL_H
#define HEATERCONTROL_H

#include "Arduino.h"
#include <SAMD21turboPWM.h>
#include <AutoPID.h>
#include <Wire.h>
#include <INA219.h>

class HeaterControl
{
public:
    HeaterControl(INA219::t_i2caddr addr);
    ~HeaterControl();
    void setupHeaterControl(int IN1, int IN2);
    void setPWMHeater(int pwm);
    void setVoltageHeater(float voltage);
    void setTemperatureHeater(float temperature);
    bool setCurrentHeater(float current, bool automatic);
    float getCurrentHeater();
    float getVoltageHeater();
    float getResistenceHeater();
    float getTemperatureHeater();
    void setErrorCurrentHeater(float error);
    float getMeasureCurrentHeater();
    void offHeaterControl();
    void resetControlHeater();

private:
    int pinControl1;
    int pinControl2;
    INA219 monitorHeater;
    double currentHeater;
    double setPointCurrentHeater;
    double valuePWM;
    float calibMonitorHeater;
    AutoPID controlPID;
};

#endif