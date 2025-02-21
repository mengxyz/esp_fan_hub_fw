#include <Arduino.h>
#include <SensorData.h>
#include <I2C_eeprom.h>
#include <ArduinoJson.h>
#include <Thermister.h>
#include <VoltageSensor.h>
#include <MolexPowerMeter.h>
#include <FanControl.h>
#include <ConfigData.h>
#include <AT24CX.h>

#if !defined(DATA_STORE_H)
#define DATA_STORE_H

#define RESET_THRESHOLD 5000

class DataStore
{
    static void IRAM_ATTR RESET_FLAG();
    static DataStore *instance;

private:
    String serializedSensorDataString();
    String serializedConfigDataString();
    void serializedSensorDataDoc();
    void serializedConfigDataDoc();
    bool initEEPROM();
    I2C_eeprom ee;
    AT24CX at24cx = AT24CX();
    bool eepromReady = false;
    uint8_t resetPin;
    volatile bool resetFlag = false;
    volatile unsigned long resetPressTime = 0;
    volatile uint16_t resetThreshold = RESET_THRESHOLD;
    JsonDocument sensorDataDoc;
    JsonDocument configDataDoc;
    void loadConfigData();
    void setDefaultConfigData();

public:
    DataStore(uint8_t eeAddress = 0x50, int eeSize = I2C_DEVICESIZE_24LC256, uint8_t resetPin = -1);
    void setResetPin(uint8_t resetPin) { this->resetPin = resetPin; }
    SensorData sensorData;
    ConfigData configData;
    String getSensorDataJson();
    String getConfigDataJson();
    JsonDocument getConfigDataDoc() {
        serializedConfigDataDoc();
        return configDataDoc;
    }
    void serializedSensorDataBuffer(char *buf);
    void begin();
    void printSensorData();
    void setThermisterData(Thermister &thermister);
    void setVoltageSensorData(VoltageSensor &voltageSensor);
    void setMolexPowerData(MolexPowerMeter &molexPowerMeter);
    void saveConfigData();
    void saveDefaultConfigData();
    void service();
};

#endif // DATA_STORE_H
