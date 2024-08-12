#include <BoardTempSensor.h>

BoardTempSensor::BoardTempSensor(SHTSensor::SHTSensorType sensorType): sht(sensorType)
{
}

void BoardTempSensor::begin()
{
    sht.init();
}

void BoardTempSensor::readSensors(float &temp, float &humi, float &cpuTemp)
{
    if (!sht.readSample())
    {
        temp = 0;
        humi = 0;
    } else {
        temp = sht.getTemperature();
        humi = sht.getHumidity();
    }
    cpuTemp = temperatureRead();
}