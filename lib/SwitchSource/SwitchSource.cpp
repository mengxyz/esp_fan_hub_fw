// lib/SwitchSource/SwitchSource.cpp

#include <SwitchSource.h>
#include <Arduino.h>
#include <Wire.h>
#include <PCF8574.h>

bool SwitchSource::begin()
{
    pcf.pinMode(PWM_SW_CH_1, OUTPUT);
    pcf.pinMode(PWM_SW_CH_2, OUTPUT);
    pcf.pinMode(PWM_SW_CH_3, OUTPUT);
    pcf.pinMode(PWM_SW_CH_4, OUTPUT);
    pcf.pinMode(PWM_SW_CH_5, OUTPUT);
    pcf.pinMode(ARGB_SW, OUTPUT);

    pcf.pinMode(SW_1, INPUT);
    pcf.pinMode(SW_2, INPUT);

    if (pcf.begin())
    {
        pcf.digitalWrite(PWM_SW_CH_1, LOW);
        pcf.digitalWrite(PWM_SW_CH_2, LOW);
        pcf.digitalWrite(PWM_SW_CH_3, LOW);
        pcf.digitalWrite(PWM_SW_CH_4, LOW);
        pcf.digitalWrite(PWM_SW_CH_5, LOW);
        pcf.digitalWrite(ARGB_SW, LOW);
        ready = true;
        Serial.println("SwitchSource Ready");
        return true;
    }
    notReady();
    return false;
}

void SwitchSource::toggle(InputSource source)
{
    if (!this->ready)
    {
        notReady();
        return;
    }
    if (pcf.digitalRead(source) == LOW)
    {
        pcf.digitalWrite(source, HIGH);
        return;
    }
    pcf.digitalWrite(source, LOW);
}

int SwitchSource::readInputState(InputSource source)
{
    if (!ready)
    {
        notReady();
        return -1;
    }
    return pcf.digitalRead(source);
}

SwitchSource::SwitchSource() : pcf(PCF8574_I2C_ADDRESS)
{
}

int SwitchSource::readSwitchState(SwSource source)
{
    if (!ready)
    {
        notReady();
        return -1;
    }

    return pcf.digitalRead(source);
}

void SwitchSource::notReady()
{
    Serial.println("SwitchSource not ready");
}
