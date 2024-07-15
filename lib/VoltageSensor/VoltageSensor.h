#include <Adafruit_INA219.h>
#include <SensorData.h>

#if !defined(VOLTAGE_SENSOR_H)
#define VOLTAGE_SENSOR_H

#define INA219_FIVE_VOLT_I2C_ADDRESS 0x45
// #define INA219_FIVE_VOLT_I2C_ADDRESS 0x40
#define INA219_TWELVE_VOLT_I2C_ADDRESS 0x41

class VoltageSensor
{
private:
    Adafruit_INA219 inaFiveVolt;
    Adafruit_INA219 inaTwelveVolt;
    bool fiveVoltReady = false;
    bool twelveVoltReady = false;
    void readInaData(Adafruit_INA219 &ina219, Ina219Data *data);

public:
    VoltageSensor();
    void begin();
    void read5V(Ina219Data *data);
    void read12V(Ina219Data *data);
};

#endif // VOLTAGE_SENSOR_H
