#ifndef FANCONTROL_H
#define FANCONTROL_H

#include "Arduino.h"

class FanControl
{
public:
    FanControl();
    ~FanControl();
    void setupFanControl( int pinControl);
    void setOnOffFan(bool OnOff);
private:
    int pinOnOff;
};

#endif