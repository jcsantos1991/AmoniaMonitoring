#line 1 "c:\\Users\\MinosPortatil2\\OneDrive - URV\\Amonnia Monitoring\\AmoniaMonitoring\\StoreMeasurements.h"
#ifndef STOREMEASUREMENTS_H
#define STOREMEASUREMENTS_H

#include "Arduino.h"
#include <SD.h>
#include <RTCZero.h>

class StoreMeasurements
{
public:
    StoreMeasurements();
    ~StoreMeasurements();
    bool setupStoreMeasurements(int csSD);
    bool newFileMeasurements(String &nameFile, RTCZero &clock);
    bool addMeasurement(String nameFile, RTCZero &clock, float * measurements, int numberMeasurements, String &dataLog);
    bool getMeasurements(String nameFile, unsigned long pos, String & dataMeasurement);
    bool deleteFileMeasurements(String nameFile);
    String to2digits(int number);
    String to4digitsFloat(float number);
private:

};

#endif
