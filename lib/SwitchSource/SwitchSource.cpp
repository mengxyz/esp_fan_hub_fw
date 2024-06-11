// lib/SwitchSource/SwitchSource.cpp

#include <SwitchSource.h>

bool SwitchSource::begin()
{
    this->pcf.pinMode(PWM_SW_CH_1, OUTPUT);
    this->pcf.pinMode(PWM_SW_CH_2, OUTPUT);
    this->pcf.pinMode(PWM_SW_CH_3, OUTPUT);
    this->pcf.pinMode(PWM_SW_CH_4, OUTPUT);
    this->pcf.pinMode(PWM_SW_CH_5, OUTPUT);
    this->pcf.pinMode(ARGB_SW, OUTPUT);

    this->pcf.pinMode(SW_1, INPUT);
    this->pcf.pinMode(SW_2, INPUT);

    if (this->pcf.begin())
    {
        this->pcf.digitalWrite(PWM_SW_CH_1, LOW);
        this->pcf.digitalWrite(PWM_SW_CH_2, LOW);
        this->pcf.digitalWrite(PWM_SW_CH_3, LOW);
        this->pcf.digitalWrite(PWM_SW_CH_4, LOW);
        this->pcf.digitalWrite(PWM_SW_CH_5, LOW);
        this->pcf.digitalWrite(ARGB_SW, LOW);
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

void SwitchSource::setSource(InputSource source, uint8_t state)
{
    this->pcf.digitalWrite(source, state);
}

void SwitchSource::initSource(uint8_t(& fanSource)[5], uint8_t argbSource)
{
    for (int i = 0; i < 5; i++)
    {
        this->setSource(i, fanSource[i]);
    }
    this->setSource(ARGB_SW, argbSource);
}

void SwitchSource::setSource(uint8_t pin, uint8_t state)
{
    this->pcf.digitalWrite(pin, state);
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

void SwitchSource::readState(uint8_t (&fanSource)[5], uint8_t &argbSource)
{

    if (!ready)
    {
        notReady();
        return;
    }

    fanSource[0] = pcf.digitalRead(PWM_SW_CH_1);
    fanSource[1] = pcf.digitalRead(PWM_SW_CH_2);
    fanSource[2] = pcf.digitalRead(PWM_SW_CH_3);
    fanSource[3] = pcf.digitalRead(PWM_SW_CH_4);
    fanSource[4] = pcf.digitalRead(PWM_SW_CH_5);
    argbSource = pcf.digitalRead(ARGB_SW);
}

void SwitchSource::notReady()
{
    Serial.println("SwitchSource not ready");
}
