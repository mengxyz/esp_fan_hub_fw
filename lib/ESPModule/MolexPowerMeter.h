#if !defined(MOLEX_POWER_METER_H)
#define MOLEX_POWER_METER_H

#include <Arduino.h>
#include <INA3221.h>
#include <BaseModule.h>
#include <SensorData.h>
#include <UserConfig.h>
#include <Debuger.h>

#define SHUNT_RESISTANCE 0.005

class MolexPowerMeter
{
private:
    INA3221 ina5V;
    INA3221 ina12V;
    bool is5VReady = false;
    bool is12VReady = false;
public:
    MolexPowerMeter(uint8_t addr5V = INA3221_MOLEX_5V_I2C_ADDRESS, uint8_t addr12V = INA3221_MOLEX_12_I2C_ADDRESS);
    void begin();
    void readPower(MolexPowerData &data);
};

#endif // MOLEX_POWER_METER_H
