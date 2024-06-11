#include <DataStore.h>

void DataStore::begin()
{
    this->initEEPROM();
    this->loadConfigData();
}

String DataStore::serializedSensorDataString()
{
    serializedSensorDataDoc();
    String output;
    serializeJson(sensorDataDoc, output);
    return output;
}

void DataStore::serializedSensorDataDoc()
{
    // Sht40Data
    sensorDataDoc["sht40"]["temp"] = sensorData.sht40.temp;
    sensorDataDoc["sht40"]["humi"] = sensorData.sht40.humi;

    // ThermistorData
    sensorDataDoc["thermistor"]["thermistor1"] =
        sensorData.thermistor.thermistor1;
    sensorDataDoc["thermistor"]["thermistor2"] =
        sensorData.thermistor.thermistor2;

    // VoltageData - Ina219Data
    sensorDataDoc["voltage"]["twelveVolt"]["current"] =
        sensorData.voltage.twelveVolt.current;
    sensorDataDoc["voltage"]["twelveVolt"]["voltage"] =
        sensorData.voltage.twelveVolt.voltage;
    sensorDataDoc["voltage"]["twelveVolt"]["power"] =
        sensorData.voltage.twelveVolt.power;
    sensorDataDoc["voltage"]["fiveVolt"]["current"] =
        sensorData.voltage.fiveVolt.current;
    sensorDataDoc["voltage"]["fiveVolt"]["voltage"] =
        sensorData.voltage.fiveVolt.voltage;
    sensorDataDoc["voltage"]["fiveVolt"]["power"] =
        sensorData.voltage.fiveVolt.power;

    // FanInputSource
    for (int i = 0; i < 5; i++)
    {
        sensorDataDoc["fanSource"][i] = configData.fanSource[i];
    }
    sensorDataDoc["argbSource"] = configData.argb.source;

    // FanData
    for (int i = 0; i < 5; i++)
    {
        sensorDataDoc["fanData"]["freqs"][i] = sensorData.fanData.freq[i];
        sensorDataDoc["fanData"]["duties"][i] = configData.fanDuty[i];
        sensorDataDoc["fanData"]["rpms"][i] =
            sensorData.fanData.freq[i] * 60 / 2;
    }
}

void DataStore::serializedConfigDataDoc()
{
    configDataDoc["ssid"] = configData.ssid;
    configDataDoc["password"] = configData.password;
    configDataDoc["local_ip"] = configData.local_ip;
    configDataDoc["gateway"] = configData.gateway;
    configDataDoc["subnet"] = configData.subnet;
    configDataDoc["dns1"] = configData.dns1;
    configDataDoc["dns2"] = configData.dns2;
    configDataDoc["auth_user"] = configData.auth_user;
    configDataDoc["auth_password"] = configData.auth_password;
    for (int i = 0; i < 5; i++)
    {
        if (configData.fanSource[i] == 0)
        {
            configDataDoc["fan_source"][i] = 0;
            configDataDoc["fan_duty"][i] = 0;
        }
        else
        {
            configDataDoc["fan_source"][i] = configData.fanSource[i];
            configDataDoc["fan_duty"][i] = configData.fanDuty[i];
        }
    }
    configDataDoc["argb"]["mode"] = configData.argb.mode;
    configDataDoc["argb"]["speed"] = configData.argb.speed;
    configDataDoc["argb"]["brightness"] = configData.argb.brightness;
    configDataDoc["argb"]["source"] = configData.argb.source;
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

    strncpy(configData.ssid, env_wifi_ssid, sizeof(configData.ssid));
    strncpy(configData.password, env_wifi_password, sizeof(configData.password));
    strncpy(configData.local_ip, env_local_ip, sizeof(configData.local_ip));
    strncpy(configData.gateway, env_gateway, sizeof(configData.gateway));
    strncpy(configData.subnet, env_subnet, sizeof(configData.subnet));
    strncpy(configData.dns1, env_dns1, sizeof(configData.dns1));
    strncpy(configData.dns2, env_dns2, sizeof(configData.dns2));
    strncpy(configData.auth_user, env_auth_user, sizeof(configData.auth_user));
    strncpy(configData.auth_password, env_auth_password, sizeof(configData.auth_password));
    for (int i = 0; i < 5; i++)
    {
        configData.fanSource[i] = 0;
    }
    configData.argb.mode = 0;
    configData.argb.speed = 1000;
    configData.argb.brightness = 255;
    configData.argb.source = LOW;
}

String DataStore::getSensorDataJson() { return serializedSensorDataString(); }

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
    sensorData.thermistor.thermistor1 =
        thermister.readAdc(ThermisterChannel::CHANNEL_0);
    sensorData.thermistor.thermistor2 =
        thermister.readAdc(ThermisterChannel::CHANNEL_1);
}

void DataStore::setVoltageSensorData(VoltageSensor &voltageSensor)
{
    voltageSensor.read5V(&sensorData.voltage.fiveVolt);
    voltageSensor.read12V(&sensorData.voltage.twelveVolt);
}

void DataStore::setFanData(FanControl &fanControl)
{
    fanControl.readFanData(&sensorData.fanData);
}

void DataStore::loadConfigData()
{
    Serial.println("Loading config data");
    this->ee.readBlock(0, (uint8_t *)&configData, sizeof(configData));
    Serial.println("Config data loaded");
    this->serializedConfigDataDoc();
    serializeJsonPretty(configDataDoc, Serial);
    Serial.println("");
}

void DataStore::saveConfigData()
{
    this->ee.setBlock(0, 0, sizeof(configData));
    this->ee.writeBlock(0, (uint8_t *)&configData, sizeof(configData));
    Serial.println("Config data saved");
}