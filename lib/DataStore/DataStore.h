#include <Arduino.h>
#include <SensorData.h>
#include <I2C_eeprom.h>
#include <ArduinoJson.h>
#include <SensirionI2cSht4x.h>
#include <Thermister.h>
#include <VoltageSensor.h>
#include <FanControl.h>
#include <ConfigData.h>

#if !defined(DATA_STORE_H)
#define DATA_STORE_H

#define EEPROM_I2C_ADDRESS 0x50

class DataStore
{
private:
    String serializedSensorDataString();
    void serializedSensorDataDoc();
    void serializedConfigDataDoc();
    bool initEEPROM();
    I2C_eeprom ee;
    bool eepromReady = false;
    JsonDocument sensorDataDoc;
    JsonDocument configDataDoc;
    void loadConfigData();
    void setDefaultConfigData();

public:
    DataStore();
    SensorData sensorData;
    ConfigData configData;
    String getSensorDataJson();
    void begin();
    void printSensorData();
    void setThermisterData(Thermister &thermister);
    void setVoltageSensorData(VoltageSensor &voltageSensor);
    void saveConfigData();
    void saveDefaultConfigData();
};

#endif // DATA_STORE_H
