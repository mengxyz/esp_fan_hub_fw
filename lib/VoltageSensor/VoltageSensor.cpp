#include <VoltageSensor.h>

VoltageSensor::VoltageSensor() : inaFiveVolt(INA219_FIVE_VOLT_I2C_ADDRESS), inaTwelveVolt(INA219_TWELVE_VOLT_I2C_ADDRESS)
{
}

void VoltageSensor::begin()
{
    if (!inaFiveVolt.begin(&Wire))
    {
        Serial.println("VoltageSensor 5V not ready");
    }
    else
    {
        Serial.println("VoltageSensor 5V ready");
        inaFiveVolt.setCalibration_16V_10A();
        fiveVoltReady = true;
    }
    if (!inaTwelveVolt.begin(&Wire))
    {
        Serial.println("VoltageSensor 12V not ready");
    }
    else
    {
        Serial.println("VoltageSensor 12V ready");
        inaTwelveVolt.setCalibration_16V_10A();
        twelveVoltReady = true;
    }
}

void VoltageSensor::read5V(Ina219Data *data)
{
    if (fiveVoltReady)
    {
        readInaData(inaFiveVolt, data);
    }
    else
    {
        Serial.println("VoltageSensor 5V not initialized");
    }
}

void VoltageSensor::read12V(Ina219Data *data)
{
    if (twelveVoltReady)
    {
        readInaData(inaTwelveVolt, data);
    }
    else
    {
        Serial.println("VoltageSensor 12V not initialized");
    }
}

void VoltageSensor::readInaData(Adafruit_INA219 &ina219, Ina219Data *data)
{
    data->voltage = ina219.getBusVoltage_V();
    data->current = ina219.getCurrent_mA();
    data->power = ina219.getPower_mW();
}
