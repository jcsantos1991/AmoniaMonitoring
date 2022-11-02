#include "TempHumiSensor.h"

TempHumiSensor::TempHumiSensor()
{
}

TempHumiSensor::~TempHumiSensor()
{
}

void TempHumiSensor::setupSensor()
{
    Wire.begin();
    if(!(sht.init())){
        Serial.println("SHT85 sensor failed, or not present");
    }
 
}

bool TempHumiSensor::readSampleSensor()
{
   return sht.readSample();
}

float TempHumiSensor::getHumiditySensor()
{
    return sht.getHumidity();
}
    
float TempHumiSensor::getTempSensor()
{
   return sht.getTemperature();
}
