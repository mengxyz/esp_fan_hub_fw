#include <BoardTempSensor.h>

BoardTempSensor::BoardTempSensor()
{
}

void BoardTempSensor::begin()
{
    if (initSht3x())
    {
        sensorType = SensorType::SHT3X;
        isReady = true;
        Serial.println("BoardTempSensor SHT30 ready");
        return;
    }
    if (initSht4x())
    {
        sensorType = SensorType::SHT4X;
        isReady = true;
        Serial.println("BoardTempSensor SHT40 ready");
        return;
    }
}

void BoardTempSensor::readSensors(float &temp, float &humi)
{
    if (!isReady)
    {
        Serial.println("BoardTempSensor not ready");
        return;
    }
    if (sensorType == SensorType::SHT3X)
    {
        sht3x.blockingReadMeasurement(temp, humi);
    }
    if (sensorType == SensorType::SHT4X)
    {
        sht4x.measureMediumPrecision(temp, humi);
    }
}

bool BoardTempSensor::initSht3x()
{
    sht3x.begin(Wire, SHT30_I2C_ADDR_44);
    sht3x.stopMeasurement();
    delay(1);
    sht3x.softReset();
    delay(10);
    uint16_t aStatusRegister = 0u;
    error = sht3x.readStatusRegister(aStatusRegister);
    if (error != NO_ERROR)
    {
        Serial.println("SHT30 init failed");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return false;
    }
    return true;
}

bool BoardTempSensor::initSht4x()
{
    sht4x.begin(Wire, SHT40_I2C_ADDR_44);
    sht4x.softReset();
    delay(10);
    error = sht4x.serialNumber(serialNumber);
    if (error != NO_ERROR)
    {
        Serial.println("SHT40 init failed");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return false;
    }
    return true;
}