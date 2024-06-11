#include "Thermister.h"

float Thermister::calculateTemperature(float resistance)
{
    return (1.0 / ((log(R0 / resistance) / BETA) + (1.0 / T0))) - 273.15;
}

Thermister::Thermister()
{
}

void Thermister::begin()
{
    if (!this->ads.begin(ADS1115_I2C_ADDRESS, &Wire))
    {
        Serial.println("Thermister ready");
        return;
    }
    Serial.println("Thermister ready");
    ready = true;
}

float Thermister::readTemp(ThermisterChannel channel)
{
    if (!ready)
    {
        Serial.println("Thermister not ready");
        return 0.0;
    }
    int16_t adc_value = ads.readADC_SingleEnded(channel);
    adc_value += ADJUST_ADC_VALUE;
    float voltage = adc_value * (ADC_REF_VOLTAGE / 32767.0);
    float resistance = R0 * (ADC_REF_VOLTAGE / voltage - 1.0);
    float tempC = calculateTemperature(resistance);
    return tempC += TEMP_OFFSET;
}

float Thermister::readResistance(ThermisterChannel channel)
{
    return 0.0f;
}

float Thermister::readAdc(ThermisterChannel channel)
{
    return this->readTemp(channel);
}
