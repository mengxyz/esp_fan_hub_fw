#if !defined(MOLEX_POWER_METER_H)
#define MOLEX_POWER_METER_H

#include <Arduino.h>
#include <INA3221.h>
#include <BaseModule.h>
#include <SensorData.h>

#define INA3221_I2C_ADDRESS 0x43 // I2C address

class MolexPowerMeter : public BaseModule
{
private:
    INA3221 ina3221;
public:
    MolexPowerMeter();
    void begin();
    void readPower(Ina219Data (&data)[3]);
};

#endif // MOLEX_POWER_METER_H
