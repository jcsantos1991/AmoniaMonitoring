#line 1 "c:\\Users\\MinosPortatil2\\OneDrive - URV\\Amonnia Monitoring\\AmoniaMonitoring\\TempHumiSensor.h"
#ifndef TEMPHUMISENSOR_H
#define TEMPHUMISENSOR_H

#include "Arduino.h"
#include <Wire.h>
#include <SHTSensor.h>


class TempHumiSensor
{
public:
    TempHumiSensor();
    ~TempHumiSensor();
    void setupSensor();
    bool readSampleSensor();
    float getHumiditySensor();
    float getTempSensor();
private:
    SHTSensor sht;
};

#endif
