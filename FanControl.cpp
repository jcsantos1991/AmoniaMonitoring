#include "FanControl.h"

FanControl::FanControl()
{
}

FanControl::~FanControl()
{
}

void FanControl::setupFanControl(int pinControl)
{
    pinOnOff = pinControl;
    pinMode(pinOnOff, OUTPUT);
}

void FanControl::setOnOffFan(bool OnOff)
{
    if (OnOff == true)
    {
        digitalWrite(pinOnOff, HIGH);
    }
    else
    {
        digitalWrite(pinOnOff, LOW);
    }
    
}