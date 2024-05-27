#if !defined(SENSOR_DATA_H)
#define SENSOR_DATA_H

struct FanData
{
    uint8_t freqs[5];
    uint8_t duties[5];
    uint8_t rpms[5];
};


struct FanInputSource
{
    uint8_t ch1;
    uint8_t ch2;
    uint8_t ch3;
    uint8_t ch4;
    uint8_t ch5;
    uint8_t argb;
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

struct Sht40Data
{
    float temp;
    float humi;
};

struct ThermistorData
{
    float thermistor1;
    float thermistor2;
};

struct SensorData
{
    Sht40Data sht40;
    ThermistorData thermistor;
    VoltageData voltage;
    FanInputSource fanInputSource;
    FanData fanData;
};

#endif // SENSOR_DATA_H
