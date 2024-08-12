#if !defined(MOLEX_POWER_METER_H)
#define MOLEX_POWER_METER_H

#include <Arduino.h>
#include <INA3221.h>
#include <BaseModule.h>
#include <SensorData.h>

class MolexPowerMeter : public BaseModule
{
private:
    INA3221 ina3221;
public:
    MolexPowerMeter(uint8_t address = 0x43);
    void begin();
    void readPower(Ina219Data (&data)[3]);
};

#endif // MOLEX_POWER_METER_H
