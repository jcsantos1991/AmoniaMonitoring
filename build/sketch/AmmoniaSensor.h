#line 1 "c:\\Users\\MinosPortatil2\\OneDrive - URV\\Amonnia Monitoring\\AmoniaMonitoring\\AmmoniaSensor.h"
#ifndef AMMONIASENSOR_H
#define AMMONIASENSOR_H

#include "Arduino.h"
#include "AD524X.h"

class AmmoniaSensor
{
public:
    
    AmmoniaSensor(int addrPot1,int addrPot2);
    ~AmmoniaSensor();
    void setupSensor (int chanelADCSetup, float offsetVoltageSetup, float r1Setup, float r2Setup, float rgSetup, float refVoltageSetup);
    void setOffsetVoltage(float voltage);
    void setR1(float resistencie);
    void setRG(float resistencie);
    float getR1();
    float getSensorResistence();
    float getR2Voltage();
    float getSensorVoltage();
    float getMeanSensorResistence();
    float meanSensorResistence(int count);
    bool calculateR1(float resistencie);

private:
    int chanelADC = A0;
    float r1;
    float r2;
    float offsetVoltage;
    float refVoltage;
    int dataAdc;
    float r2Voltage;
    float bridgeVoltage;
    float gainAmp;
    float dataVoltAdc;
    float sensorVoltage;
    float sensorResistence;
    float mean;
    AD5241 potentiometreR1;
    AD5241 potentiometreRg;
};

#endif
