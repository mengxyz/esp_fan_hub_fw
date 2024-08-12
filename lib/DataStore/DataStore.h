#include <Arduino.h>
#include <SensorData.h>
#include <I2C_eeprom.h>
#include <ArduinoJson.h>
#include <Thermister.h>
#include <VoltageSensor.h>
#include <FanControl.h>
#include <ConfigData.h>

#if !defined(DATA_STORE_H)
#define DATA_STORE_H

class DataStore
{
    static void IRAM_ATTR RESET_FLAG();
    static DataStore *instance;
private:
    String serializedSensorDataString();
    void serializedSensorDataDoc();
    void serializedConfigDataDoc();
    bool initEEPROM();
    I2C_eeprom ee;
    bool eepromReady = false;
    uint8_t resetPin;
    bool resetFlag = false;
    JsonDocument sensorDataDoc;
    JsonDocument configDataDoc;
    void loadConfigData();
    void setDefaultConfigData();

public:
    DataStore(uint8_t eeAddress = 0x50, int eeSize = I2C_DEVICESIZE_24LC256, uint8_t resetPin = -1);
    SensorData sensorData;
    ConfigData configData;
    String getSensorDataJson();
    void begin();
    void printSensorData();
    void setThermisterData(Thermister &thermister);
    void setVoltageSensorData(VoltageSensor &voltageSensor);
    void saveConfigData();
    void saveDefaultConfigData();
    void service();
};

#endif // DATA_STORE_H
