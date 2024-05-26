// lib/SwitchSource/SwitchSource.h

#ifndef SWITCH_SOURCE_h
#define SWITCH_SOURCE_h

#include <Arduino.h>
#include <Wire.h>
#include <PCF8574.h>

#define PCF8574_I2C_ADDRESS 0x39

enum InputSource
{
    PWM_SW_CH_1 = P6,
    PWM_SW_CH_2 = P5,
    PWM_SW_CH_3 = P4,
    PWM_SW_CH_4 = P2,
    PWM_SW_CH_5 = P0,
    ARGB_SW = P1,
};

enum SwSource
{
    SW_1 = P3,
    SW_2 = P7
};

class SwitchSource
{
    bool ready = false;

public:
    PCF8574 pcf;
    SwitchSource();
    bool begin();
    void toggle(InputSource source);
    int readInputState(InputSource source);
    int readSwitchState(SwSource source);

private:
    void notReady();
};

#endif