#include <MolexPowerMeter.h>

// MolexPowerMeter::MolexPowerMeter(uint8_t addr5V, uint8_t addr12V): ina3221(address, &Wire)
// {
// }

MolexPowerMeter::MolexPowerMeter(uint8_t addr5V, uint8_t addr12V) : ina5V(addr5V, &Wire), ina12V(addr12V, &Wire)
{
}

void MolexPowerMeter::begin()
{
    if (!ina5V.begin())
    {
        FANHUB_DEBUG_PRINTLN("MolexPowerMeter 5V not ready");
    }
    else
    {
        ina5V.setShuntR(0, SHUNT_RESISTANCE);
        ina5V.setShuntR(1, SHUNT_RESISTANCE);
        ina5V.setShuntR(2, SHUNT_RESISTANCE);
        FANHUB_DEBUG_PRINTLN("MolexPowerMeter 5V ready");
        is5VReady = true;
    }

    if (!ina12V.begin())
    {
        FANHUB_DEBUG_PRINTLN("MolexPowerMeter 12V not ready");
    }
    else
    {
        ina12V.setShuntR(0, SHUNT_RESISTANCE);
        ina12V.setShuntR(1, SHUNT_RESISTANCE);
        ina12V.setShuntR(2, SHUNT_RESISTANCE);
        FANHUB_DEBUG_PRINTLN("MolexPowerMeter 12V ready");
        is12VReady = true;
    }
}

void MolexPowerMeter::readPower(MolexPowerData &data)
{
    for (int ch = 0; ch < 3; ch++)
    {
        if (is5VReady)
        {
            data.fiveVolt[ch].voltage = ina5V.getBusVoltage_mV(ch) / 1000;
            data.fiveVolt[ch].current = ina5V.getCurrent_mA(ch);
            data.fiveVolt[ch].power = ina5V.getPower_mW(ch);
        }
        if (is12VReady)
        {
            data.twelveVolt[ch].voltage = ina12V.getBusVoltage_mV(ch) / 1000;
            data.twelveVolt[ch].current = ina12V.getCurrent_mA(ch);
            data.twelveVolt[ch].power = ina12V.getPower_mW(ch);
        }
    }
}
