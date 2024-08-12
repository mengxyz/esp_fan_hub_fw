#if !defined(SENSOR_DATA_H)
#define SENSOR_DATA_H

struct FanData
{
    uint8_t freq[5];
    uint8_t duty[5];
    uint16_t rpm[5];
};


struct Ina219Data
{
    float current;
    float voltage;
    float power;
};

struct VoltageData
{
    Ina219Data twelveVolt;
    Ina219Data fiveVolt;
};

struct BoardTempData
{
    float temp;
    float humi;
    float cpuTemp;
};

struct ThermistorData
{
    int16_t ch0Adc;
    int16_t ch1Adc;
    float ch0Resistance;
    float ch1Resistance;
    float ch0Voltage;
    float ch1Voltage;
    float ch0Temp;
    float ch1Temp;
};

struct RamData
{
    uint32_t free;
    uint32_t total;
};

struct SensorData
{
    BoardTempData boardTemp;
    ThermistorData thermistor;
    VoltageData voltage;
    FanData fanData;
    Ina219Data molexPower[3];
    RamData ram;
    RamData psram;
};

#endif // SENSOR_DATA_H
