#include "Thermister.h"

// float R1 = 100000;
float ch0c1 = -2.286436708e-03, ch0c2 = 6.124594487e-04, ch0c3 = -9.856614496e-07; // 100K 4250
float ch1c1 = 1.710095819e-03, ch1c2 = 0.9672732619e-04, ch1c3 = 2.814661846e-07;  // 10K 4250

float Thermister::calculateTemperature(ThermisterChannel channel, float resistance)
{
    // float temperature = resistance / nominal_resistance; // (R/Ro)
    // temperature = log(temperature); // ln(R/Ro)
    // temperature /= b_value; // 1/B * ln(R/Ro)
    // temperature += 1.0 / nominal_temperature; // + (1/To)
    // temperature = 1.0 / temperature; // Invert
    // temperature -= 273.15; // Convert to Celsius
    // return temperature;
    float logR2 = log(resistance);
    if (channel == CHANNEL_0)
    {
        return (1.0 / (ch0c1 + ch0c2 * logR2 + ch0c3 * logR2 * logR2 * logR2)) - 273.15;
    }
    return (1.0 / (ch1c1 + ch1c2 * logR2 + ch1c3 * logR2 * logR2 * logR2)) - 273.15;
}

Thermister::Thermister()
{
}

void Thermister::begin(uint8_t addr)
{
    if (!this->ads.begin(addr, &Wire))
    {
        DEBUG_PRINTLN("Thermister not ready");
        return;
    }
    DEBUG_PRINTLN("Thermister ready");
    ads.setGain(GAIN_ONE);
    ready = true;
}

float Thermister::readTemp(ThermisterChannel channel)
{
    if (!ready)
    {
        DEBUG_PRINTLN("Thermister not ready");
        return 0.0;
    }
    float resistance = readResistance(channel);
    return calculateTemperature(channel, resistance);
}

void Thermister::readData(ThermisterChannel channel, int16_t &adc_value, float &voltage, float &tempC, float &resistance)
{
    if (!ready)
    {
        DEBUG_PRINTLN("Thermister not ready");
        return;
    }

    adc_value = ads.readADC_SingleEnded(channel);
    if (adc_value == 0)
    {
        return;
    }

    // Ensure ADC value is within the expected range
    if (adc_value > MAX_ADC_VALUE)
    {
        return;
    }

    voltage = (adc_value * 0.1875) / 1000;

    resistance = (series_resistor / ((MAX_ADC_VALUE / adc_value) - 1.0)) + resistance_offset;

    if (resistance <= 0)
    {
        return;
    }

    tempC = calculateTemperature(channel, resistance);
}

float Thermister::readResistance(ThermisterChannel channel)
{
    if (!ready)
    {
        DEBUG_PRINTLN("Thermister not ready");
        return 0.0;
    }

    int16_t adc_value = ads.readADC_SingleEnded(channel);
    if (adc_value == 0)
    {
        DEBUG_PRINTLN("ADC value is zero");
        return 0.0;
    }

    // Ensure ADC value is within the expected range
    if (adc_value > MAX_ADC_VALUE)
    {
        DEBUG_PRINT("ADC value out of range: ");
        DEBUG_PRINTLN(adc_value);
        return 0.0;
    }

    float resistance = series_resistor / ((MAX_ADC_VALUE / adc_value) - 1.0);
    return resistance;
}

float Thermister::readAdc(ThermisterChannel channel)
{
    return this->readTemp(channel);
}