#if !defined(THERMISTER_H)
#define THERMISTER_H

#include <Adafruit_ADS1X15.h>
#include <Arduino.h>
#include <Wire.h>

#define R0 10000.0           // Resistor value in ohms
#define BETA 3950.0         // Beta value of the NTC thermistor
#define T0 298.15           // Temperature in Kelvin at R0 (25°C)
#define ADC_REF_VOLTAGE 5.0 // Reference voltage for ADC
#define ADJUST_ADC_VALUE 0  // Reference voltage for ADC
#define TEMP_OFFSET -11     // Offset for temperature readings

#define ADS1115_I2C_ADDRESS 0x48

enum ThermisterChannel
{
    CHANNEL_0 = 0,
    CHANNEL_1 = 1,
};

class Thermister
{
private:
    bool ready = false;
    float calculateTemperature(float resistance);

public:
    Adafruit_ADS1115 ads;
    Thermister();
    void begin();
    float readTemp(ThermisterChannel channel);
};

#endif // THERMISTER_H
