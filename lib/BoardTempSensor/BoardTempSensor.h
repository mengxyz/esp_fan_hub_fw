#if !defined(BOARDTEMPSENSOR_H)
#define BOARDTEMPSENSOR_H

#include <BaseModule.h>
#include <SHTSensor.h>

class BoardTempSensor : public BaseModule
{
private:
    char errorMessage[64];
    int16_t error;
    SHTSensor sht;
public:
    BoardTempSensor(SHTSensor::SHTSensorType sensorType);
    void begin();
    void readSensors(float &temp, float &humi, float &cpuTemp);
};


#endif // BOARDTEMPSENSOR_H
