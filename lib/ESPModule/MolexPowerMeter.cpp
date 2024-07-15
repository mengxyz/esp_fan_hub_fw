#include <MolexPowerMeter.h>

MolexPowerMeter::MolexPowerMeter() : ina3221(INA3221_I2C_ADDRESS, &Wire)
{
}

void MolexPowerMeter::begin()
{
    if (!ina3221.begin())
    {
        Serial.println("INA3221 not ready");
        return;
    }
    Serial.println("INA3221 ready");
    ina3221.setShuntR(0, 0.001);
    ina3221.setShuntR(1, 0.001);
    ina3221.setShuntR(2, 0.001);
    isReady = true;
}

void MolexPowerMeter::readPower(Ina219Data (&data)[3])
{
    if (!isReady)
    {
        Serial.println("MolexPowerMeter not ready");
        return;
    }

    for (int ch = 0; ch < 3; ch++)
    {
        data[ch].voltage = ina3221.getBusVoltage_mV(ch);
        data[ch].current = ina3221.getCurrent_mA(ch);
        data[ch].power = ina3221.getPower_mW(ch);
    }
}
