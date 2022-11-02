#include "AmmoniaSensor.h"
#define LSB_R1 1000000.0 / 256.0
#define FULL_SCALE_R1 1000000.0 - LSB_R1 + 60
#define LSB_RG 1000000.0 / 256.0
#define FULL_SCALE_RG 1000000.0 - LSB_R1 + 60

AmmoniaSensor::AmmoniaSensor(int addrPot1, int addrPot2) : potentiometreR1(addrPot1), potentiometreRg(addrPot2)
{
}

AmmoniaSensor::~AmmoniaSensor()
{
}

void AmmoniaSensor::setupSensor(int chanelADCSetup, float offsetVoltageSetup, float r1Setup, float r2Setup, float rgSetup, float refVoltageSetup)
{
    chanelADC = chanelADCSetup;
    r2 = r2Setup;
    refVoltage = refVoltageSetup;
    if (!potentiometreR1.begin())
    {
        Serial.println("Potentiometer R1 failed, or not present");
    }
    if (!potentiometreRg.begin())
    {
        Serial.println("Potentiometer R1 failed, or not present");
    }
    analogReadResolution(12);
    analogReference(AR_EXTERNAL);
    analogWriteResolution(10);
    setOffsetVoltage(offsetVoltageSetup);
    setR1(r1Setup);
    setRG(rgSetup);
}

void AmmoniaSensor::setOffsetVoltage(float voltage)
{
    offsetVoltage = voltage;
    int valueDac = (offsetVoltage / 3.295) * 1023;
    analogWrite(DAC0, valueDac);
}

void AmmoniaSensor::setR1(float resistencie)
{
    int valueRes;
    if (resistencie >= FULL_SCALE_R1)
    {
        r1 = FULL_SCALE_R1;
        valueRes = 255;
    }
    else
    {
        r1 = resistencie;
        valueRes = (r1 - 60) * (256.0 / 1000000.0);
    }
    potentiometreR1.write(0, valueRes);
}

float AmmoniaSensor::getR1()
{
    return r1;
}

void AmmoniaSensor::setRG(float resistencie)
{
    gainAmp = (resistencie) / r2;
    int valueRes = (resistencie - 1000 - 60) * (256.0 / 1000000.0);
    // potentiometreRg.shutDown();
    potentiometreRg.write(0, valueRes);
}
bool AmmoniaSensor::calculateR1(float resistencie)
{
    int value = 0;
    if (abs(resistencie - r1) > (5*LSB_R1))
    {
        value = (resistencie - 60) * (256.0 / 1000000.0);
        setR1(value * LSB_R1 + 60);
        return true;
    }
    return false;
}

float AmmoniaSensor::getSensorResistence()
{
    int valueDac = (offsetVoltage / 3.295) * 1023;
    analogWrite(DAC0, valueDac);
    dataAdc = analogRead(chanelADC);
    dataVoltAdc = (refVoltage / 4096.0) * dataAdc;
    sensorVoltage = offsetVoltage + (offsetVoltage / gainAmp) - (dataVoltAdc / gainAmp);
    if (sensorVoltage == 0.0)
    {
        sensorResistence = 0;
    }
    else
    {
        sensorResistence = (r1 * sensorVoltage) / (refVoltage - sensorVoltage);
    }
    return sensorResistence;
}

float AmmoniaSensor::getR2Voltage()
{
    return r2Voltage;
}

float AmmoniaSensor::getSensorVoltage()
{
    return sensorVoltage;
}

float AmmoniaSensor::getMeanSensorResistence()
{
    return mean;
}

float AmmoniaSensor::meanSensorResistence(int count)
{
    mean = (mean * (count - 1) + sensorResistence) / float(count);
    return mean;
}
