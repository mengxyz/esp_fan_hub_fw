#ifndef THERMISTER_H
#define THERMISTER_H

#include <Adafruit_ADS1X15.h>
#include <Arduino.h>
#include <Wire.h>
#include <Debuger.h>

const float nominal_resistance = 103000.0; // Nominal resistance at 25 degrees C
const float nominal_temperature = 298.15;  // 25 degrees C in Kelvin
const float b_value = 4250.0;              // Beta value of the NTC thermistor
const float series_resistor = 100000.0;    // Series resistor value
const float MAX_ADC_VALUE = 32767.0;
const float resistance_offset = -0.0;

enum ThermisterChannel
{
    CHANNEL_0 = 0,
    CHANNEL_1 = 1,
    CHANNEL_2 = 2,
    CHANNEL_3 = 3,
};

class Thermister
{
private:
    bool ready = false;
    float calculateTemperature(ThermisterChannel channel, float resistance);

public:
    Adafruit_ADS1115 ads;
    Thermister();
    void begin(uint8_t addr);
    float readTemp(ThermisterChannel channel);
    void readData(ThermisterChannel channel, int16_t &adc_value, float &voltage, float &tempC, float &resistance);
    float readResistance(ThermisterChannel channel);
    float readAdc(ThermisterChannel channel);
};

#endif // THERMISTER_H