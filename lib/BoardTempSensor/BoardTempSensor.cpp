#include <BoardTempSensor.h>
#include <Debuger.h>

BoardTempSensor::BoardTempSensor(SHTSensor::SHTSensorType sensorType): sht(sensorType)
{
}

void BoardTempSensor::begin()
{
    if(sht.init()) {
        FANHUB_DEBUG_PRINTLN("Board Temp Sensor ready");
        return;
    } 
    FANHUB_DEBUG_PRINTLN("Board Temp Sensor not ready");
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