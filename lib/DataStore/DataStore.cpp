#include <Arduino.h>
#include <SensorData.h>
#include <I2C_eeprom.h>
#include <DataStore.h>
#include <ArduinoJson.h>
#include <SensirionI2cSht4x.h>
#include <Thermister.h>
#include <VoltageSensor.h>

String DataStore::serializedSensorDataString()
{
    serializedSensorDataDoc();
    String output;
    serializeJson(sensorDataDoc, output);
    return output;
}

JsonDocument DataStore::serializedSensorDataDoc()
{
    // Sht40Data
    sensorDataDoc["sht40"]["temp"] = sensorData.sht40.temp;
    sensorDataDoc["sht40"]["humi"] = sensorData.sht40.humi;

    // ThermistorData
    sensorDataDoc["thermistor"]["thermistor1"] = sensorData.thermistor.thermistor1;
    sensorDataDoc["thermistor"]["thermistor2"] = sensorData.thermistor.thermistor2;

    // VoltageData - Ina219Data
    sensorDataDoc["voltage"]["twelveVolt"]["current"] = sensorData.voltage.twelveVolt.current;
    sensorDataDoc["voltage"]["twelveVolt"]["voltage"] = sensorData.voltage.twelveVolt.voltage;
    sensorDataDoc["voltage"]["twelveVolt"]["power"] = sensorData.voltage.twelveVolt.power;
    sensorDataDoc["voltage"]["fiveVolt"]["current"] = sensorData.voltage.fiveVolt.current;
    sensorDataDoc["voltage"]["fiveVolt"]["voltage"] = sensorData.voltage.fiveVolt.voltage;
    sensorDataDoc["voltage"]["fiveVolt"]["power"] = sensorData.voltage.fiveVolt.power;

    return sensorDataDoc;
}

bool DataStore::initEEPROM()
{
    ee.begin();
    if (!ee.isConnected())
    {
        Serial.println("EEPROM not ready");
        return false;
    }
    Serial.println("EEPROM ready");
    eepromReady = true;
    return true;
}

DataStore::DataStore() : ee(EEPROM_I2C_ADDRESS, I2C_DEVICESIZE_24LC256)
{
    sensorData.sht40.temp = 0.0;
    sensorData.sht40.humi = 0.0;
    sensorData.thermistor.thermistor1 = 0.0;
    sensorData.thermistor.thermistor2 = 0.0;
    sensorData.voltage.twelveVolt.current = 0.0;
    sensorData.voltage.twelveVolt.voltage = 0.0;
    sensorData.voltage.fiveVolt.current = 0.0;
    sensorData.voltage.fiveVolt.voltage = 0.0;
}

String DataStore::getSensorDataJson()
{
    return serializedSensorDataString();
}

void DataStore::begin()
{
    initEEPROM();
}

void DataStore::printSensorData()
{
    serializedSensorDataDoc();
    Serial.println();
    serializeJsonPretty(sensorDataDoc, Serial);
    Serial.println();
}

void DataStore::setSht40Data(SensirionI2cSht4x &sht)
{
    float temp = 0.0;
    float humi = 0.0;
    sht.measureLowestPrecision(temp, humi);
    sensorData.sht40.temp = temp;
    sensorData.sht40.humi = humi;
}

void DataStore::setThermisterData(Thermister &thermister)
{
    sensorData.thermistor.thermistor1 = thermister.readTemp(ThermisterChannel::CHANNEL_0);
    sensorData.thermistor.thermistor2 = thermister.readTemp(ThermisterChannel::CHANNEL_1);
}

void DataStore::setVoltageSensorData(VoltageSensor &voltageSensor)
{
    voltageSensor.read5V(&sensorData.voltage.fiveVolt);
    voltageSensor.read12V(&sensorData.voltage.twelveVolt);
}
