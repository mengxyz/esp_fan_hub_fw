#if !defined(SENSOR_DATA_H)
#define SENSOR_DATA_H

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
};

#endif // SENSOR_DATA_H
