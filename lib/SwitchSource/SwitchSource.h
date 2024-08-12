// lib/SwitchSource/SwitchSource.h

#ifndef SWITCH_SOURCE_h
#define SWITCH_SOURCE_h

#include <Arduino.h>
#include <Wire.h>
#include <PCF8574.h>
#include <SensorData.h>
#include <DataStore.h>

enum InputSource
{
    PWM_SW_CH_1 = P6,
    PWM_SW_CH_2 = P5,
    PWM_SW_CH_3 = P4,
    PWM_SW_CH_4 = P2,
    PWM_SW_CH_5 = P0,
    ARGB_SW = P1,
};

const uint8_t FAN_INPUT_SOURCES[] = {P6, P5, P4, P2, P0};

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
    SwitchSource(uint8_t address = 0x39);
    bool begin();
    void toggle(InputSource source);
    void setSource(InputSource source, uint8_t state);
    void initSource(uint8_t (&fanSource)[5], uint8_t argbSource);
    void setSource(uint8_t pin, uint8_t state);
    int readInputState(InputSource source);
    int readSwitchState(SwSource source);
    void readState(uint8_t (&fanSource)[5], uint8_t &argbSource);

private:
    void notReady();
};

#endif