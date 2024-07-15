#if !defined(BOARDTEMPSENSOR_H)
#define BOARDTEMPSENSOR_H

#include <BaseModule.h>
#include <SensirionI2cSht3x.h>
#include <SensirionI2cSht4x.h>

enum SensorType
{
    SHT3X = 0,
    SHT4X = 1
};


class BoardTempSensor : public BaseModule
{
private:
    char errorMessage[64];
    int16_t error;
    SensorType sensorType;
    SensirionI2cSht3x sht3x;
    SensirionI2cSht4x sht4x;
    uint32_t serialNumber = 0u;
    bool initSht3x();
    bool initSht4x();
public:
    BoardTempSensor();
    void begin();
    void readSensors(float &temp, float &humi);
};


#endif // BOARDTEMPSENSOR_H
