#include "StoreMeasurements.h"

StoreMeasurements::StoreMeasurements()
{
}

StoreMeasurements::~StoreMeasurements()
{
}

bool StoreMeasurements::setupStoreMeasurements(int csSD)
{
  if (!SD.begin(csSD))
  {
    Serial.println("Card failed, or not present");
    return false;
  }
  return true;
}

bool StoreMeasurements::newFileMeasurements(String &nameFile, RTCZero &clock)
{
  nameFile = "L" + to2digits(clock.getYear()) + to2digits(clock.getMonth()) + to2digits(clock.getDay()) + ".txt";
  File fileCVS = SD.open(nameFile, FILE_WRITE);
  if (fileCVS)
  {
    fileCVS.close();
    Serial.println("File created successfully...");
    return true;
  }
  Serial.println("Error File created...");
  return false;
}

bool StoreMeasurements::addMeasurement(String nameFile, RTCZero &clock, float *measurements, int numberMeasurements, String &dataLog)
{
  dataLog = to2digits(clock.getDay()) + "/" + to2digits(clock.getMonth()) + "/" + to2digits(clock.getYear()) + "," +
                   to2digits(clock.getHours()) + ":" + to2digits(clock.getMinutes()) + ":" + to2digits(clock.getSeconds());
  Serial.print("Clock: ");
  Serial.println(dataLog);
  for (int i = 0; i < numberMeasurements; i++)
  {
    dataLog = dataLog + "," + measurements[i]; //to4digitsFloat(measurements[i]);
  }
  File fileCVS = SD.open(nameFile, FILE_WRITE);
  if (fileCVS)
  {
    fileCVS.println(dataLog);
    fileCVS.close();
    return true;
  }
  Serial.println("Error File open...");
  return false;
}

bool StoreMeasurements::getMeasurements(String nameFile, unsigned long pos, String & dataMeasurement)
{
  File fileCVS = SD.open(nameFile, FILE_READ);
  if (fileCVS)
  {
    fileCVS.seek(pos);
    while (fileCVS.available())
    {
    dataMeasurement = dataMeasurement + (fileCVS.readStringUntil('\n'));
    dataMeasurement.remove(dataMeasurement.length()-1);
    dataMeasurement = dataMeasurement + "<br/>";
    }
    //pos = fileCVS.position();
    fileCVS.close();
    return true;
  }
  Serial.println("Error File open...");
  return false;
}

bool StoreMeasurements::deleteFileMeasurements(String nameFile)
{
  SD.remove(nameFile);
}

String StoreMeasurements::to2digits(int number)
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

String StoreMeasurements::to4digitsFloat(float number)
{
  if (number < 10)
  {
    return ("000" + String(number, 2));
  }
  else if (number < 100)
  {
    return ("00" + String(number, 2));
  }
  else if (number < 1000)
  {
    return ("0" + String(number, 2));
  }
  else
  {
    return String(number, 2);
  }
}
