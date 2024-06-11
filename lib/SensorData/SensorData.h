#if !defined(SENSOR_DATA_H)
#define SENSOR_DATA_H

struct FanData
{
    uint8_t freq[5];
    uint8_t duty[5];
    uint8_t rpm[5];
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
    FanData fanData;
};

#endif // SENSOR_DATA_H
