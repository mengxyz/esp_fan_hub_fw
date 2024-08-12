#include <Adafruit_INA219.h>
#include <SensorData.h>

#if !defined(VOLTAGE_SENSOR_H)
#define VOLTAGE_SENSOR_H

#define VOLTAGE 16.0
#define SHUNT_GAIN 0.16
#define SHUNT_RESISTANCE 0.005

class VoltageSensor
{
private:
    Adafruit_INA219 inaFiveVolt;
    Adafruit_INA219 inaTwelveVolt;
    bool fiveVoltReady = false;
    bool twelveVoltReady = false;
    void readInaData(Adafruit_INA219 &ina219, Ina219Data *data);

public:
    VoltageSensor(uint8_t fiveVoltAddr = 0x45, uint8_t twelveVoltAddr = 0x41);
    void begin();
    void read5V(Ina219Data *data);
    void read12V(Ina219Data *data);
};

#endif // VOLTAGE_SENSOR_H
