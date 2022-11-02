#line 1 "c:\\Users\\MinosPortatil2\\OneDrive - URV\\Amonnia Monitoring\\AmoniaMonitoring\\HeaterControl.cpp"
#include "HeaterControl.h"

#define OUTPUT_MIN 0
#define OUTPUT_MAX 90
#define KP 200
#define KI 50
#define KD 40
#define SHUNT_MAX_V 0.04
#define BUS_MAX_V 16.0
#define MAX_CURRENT 0.4
#define SHUNT_R 0.1
#define SHUNT_ERROR 1.22

HeaterControl::HeaterControl(INA219::t_i2caddr addr) : monitorHeater(addr),
                                                       controlPID(&currentHeater, &setPointCurrentHeater, &valuePWM, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD)
{
}

HeaterControl::~HeaterControl()
{
}

void HeaterControl::setupHeaterControl(int IN1, int IN2)
{
    pinControl1 = IN1;
    pinControl2 = IN2;
    pinMode(pinControl1, OUTPUT); // sets the pin as output
    // pinMode(pinControl2, OUTPUT); // sets the pin as output
    if (monitorHeater.begin())
    {
        Serial.println("Minitor Heater failed, or not present");
    }
    monitorHeater.configure(INA219::RANGE_16V, INA219::GAIN_1_40MV, INA219::ADC_128SAMP, INA219::ADC_128SAMP, INA219::CONT_SH_BUS);
    monitorHeater.calibrate(SHUNT_R, SHUNT_MAX_V, BUS_MAX_V, MAX_CURRENT);
    calibMonitorHeater = SHUNT_ERROR;
    // controlPID.setBangBang(0.100);
    // controlPID.setTimeStep(5000);
}

void HeaterControl::setPWMHeater(int pwmValue)
{
    analogWrite(pinControl1, (pwmValue / 100.0) * 1023);
    // digitalWrite(pinControl2, LOW);
}

void HeaterControl::setVoltageHeater(float voltage)
{
    // int pwm = (voltage / Vin) * 100;
    int pwm = 0.0232 * (voltage * voltage) + 20.698 * voltage - 0.0631;
    if (pwm > 100)
    {
        pwm = 100;
    }
    else if (pwm < 0)
    {
        pwm = 0;
    }
    setPWMHeater(pwm);
}

void HeaterControl::setTemperatureHeater(float temperature)
{
    float coef = 1;
    float voltage = temperature * coef;
    setVoltageHeater(voltage);
}

void HeaterControl::offHeaterControl()
{
    digitalWrite(pinControl1, HIGH);
    digitalWrite(pinControl2, HIGH);
}

void HeaterControl::resetControlHeater()
{
    controlPID.reset();
}

float HeaterControl::getCurrentHeater()
{
    currentHeater = (monitorHeater.shuntCurrent() / calibMonitorHeater);
    return currentHeater;
}

float HeaterControl::getVoltageHeater()
{
    return monitorHeater.busVoltage();
}

float HeaterControl::getResistenceHeater()
{
    return (getVoltageHeater() / getCurrentHeater());
}

void HeaterControl::setErrorCurrentHeater(float error)
{
    calibMonitorHeater = error;
}

bool HeaterControl::setCurrentHeater(float current, bool automatic)
{
    setPointCurrentHeater = current;
    // if (currentHeater < 0.01)
    // {
    //     setPWMHeater(0);
    // }
    // else
    // {
    if (abs(currentHeater - setPointCurrentHeater) > 0.005)
    {
        controlPID.run();
        setPWMHeater(valuePWM);
        Serial.print("PWM: ");
        Serial.println(valuePWM);
        return false;
    }
    else
    {
        if (automatic)
        {
            controlPID.run();
            setPWMHeater(valuePWM);
            Serial.print("PWM: ");
            Serial.println(valuePWM);
        }
        return true;
    }
    // }
}

float HeaterControl::getMeasureCurrentHeater()
{
    return currentHeater;
}