#include <DataStore.h>
// #include <EEPROM.h>

DataStore *DataStore::instance = nullptr;

void DataStore::begin()
{
    FANHUB_DEBUG_PRINTF("RESET_PIN %d\n", resetPin);
    if (resetPin != -1)
    {
        FANHUB_DEBUG_PRINTF("Attaching Reset pin: %d\n", resetPin);
        pinMode(resetPin, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(resetPin), RESET_FLAG, FALLING);
        delay(10);
    }
    setDefaultConfigData();
    initEEPROM();
    loadConfigData();
}

String DataStore::serializedSensorDataString()
{
    serializedSensorDataDoc();
    String output;
    serializeJson(sensorDataDoc, output);
    return output;
}

String DataStore::serializedConfigDataString()
{
    serializedConfigDataDoc();
    String output;
    serializeJson(configDataDoc, output);
    return output;
}

void DataStore::serializedSensorDataBuffer(char *buf)
{
    serializedSensorDataDoc();
    serializeJson(sensorDataDoc, buf, 256);
}

void DataStore::serializedSensorDataDoc()
{
    // RAM
    sensorDataDoc["ram"]["free"] = ESP.getFreeHeap();
    sensorDataDoc["ram"]["total"] = ESP.getHeapSize();
    sensorDataDoc["psram"]["total"] = ESP.getPsramSize();
    sensorDataDoc["psram"]["free"] = ESP.getFreePsram();
    // Sht40Data
    sensorDataDoc["boardTemp"]["temp"] = sensorData.boardTemp.temp;
    sensorDataDoc["boardTemp"]["humi"] = sensorData.boardTemp.humi;
    sensorDataDoc["boardTemp"]["cpuTemp"] = sensorData.boardTemp.cpuTemp;

    // ThermistorData
    sensorDataDoc["thermistor"]["ch0Adc"] =
        sensorData.thermistor.ch0Adc;
    sensorDataDoc["thermistor"]["ch0Resistance"] =
        sensorData.thermistor.ch0Resistance;
    sensorDataDoc["thermistor"]["ch0Voltage"] =
        sensorData.thermistor.ch0Voltage;
    sensorDataDoc["thermistor"]["ch0Temp"] = sensorData.thermistor.ch0Temp;

    sensorDataDoc["thermistor"]["ch1Adc"] =
        sensorData.thermistor.ch1Adc;
    sensorDataDoc["thermistor"]["ch1Resistance"] =
        sensorData.thermistor.ch1Resistance;
    sensorDataDoc["thermistor"]["ch1Voltage"] =
        sensorData.thermistor.ch1Voltage;
    sensorDataDoc["thermistor"]["ch1Temp"] = sensorData.thermistor.ch1Temp;

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
        sensorDataDoc["fanData"]["rpms"][i] = sensorData.fanData.rpm[i];
    }

    for (int i = 0; i < 3; i++)
    {
        sensorDataDoc["module"]["molex"]["fiveVolt"][i]["current"] = sensorData.molexPower.fiveVolt[i].current;
        sensorDataDoc["module"]["molex"]["fiveVolt"][i]["voltage"] = sensorData.molexPower.fiveVolt[i].voltage;
        sensorDataDoc["module"]["molex"]["fiveVolt"][i]["power"] = sensorData.molexPower.fiveVolt[i].power;

        sensorDataDoc["module"]["molex"]["twelveVolt"][i]["current"] = sensorData.molexPower.twelveVolt[i].current;
        sensorDataDoc["module"]["molex"]["twelveVolt"][i]["voltage"] = sensorData.molexPower.twelveVolt[i].voltage;
        sensorDataDoc["module"]["molex"]["twelveVolt"][i]["power"] = sensorData.molexPower.twelveVolt[i].power;
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
    configDataDoc["mdns"] = configData.mdns;
    for (int i = 0; i < 5; i++)
    {
        configDataDoc["fan_source"][i] = configData.fanSource[i];
        configDataDoc["fan_duty"][i] = configData.fanDuty[i];
    }
    configDataDoc["argb"]["mode"] = configData.argb.mode;
    configDataDoc["argb"]["speed"] = configData.argb.speed;
    configDataDoc["argb"]["brightness"] = configData.argb.brightness;
    configDataDoc["argb"]["source"] = configData.argb.source;
    configDataDoc["argb"]["color"] = configData.argb.color;
}

bool DataStore::initEEPROM()
{
#ifdef ESP_EEPROM
    if (!at24cx.begin())
    {
        return false;
        FANHUB_DEBUG_PRINTLN("EEPROM not ready");
    }
    FANHUB_DEBUG_PRINTLN("EEPROM ready");
    eepromReady = true;
    return true;
#else
    ee.begin();

    if (!ee.isConnected())
    {
        FANHUB_DEBUG_PRINTLN("EEPROM not ready");
        return false;
    }
    FANHUB_DEBUG_PRINTLN("EEPROM ready");
    eepromReady = true;
    return true;
#endif
}

DataStore::DataStore(uint8_t eeAddress, int eeSize, uint8_t resetPin) : ee(eeAddress, eeSize)
{
    this->resetPin = resetPin;
    instance = this;
}

String DataStore::getSensorDataJson() { return serializedSensorDataString(); }

String DataStore::getConfigDataJson()
{
    return serializedConfigDataString();
}

void DataStore::printSensorData()
{
    serializedSensorDataDoc();
    FANHUB_DEBUG_PRINTLN();
    serializeJsonPretty(sensorDataDoc, Serial);
    FANHUB_DEBUG_PRINTLN();
}

void DataStore::setThermisterData(Thermister &thermister)
{
    thermister.readData(ThermisterChannel::CHANNEL_0, sensorData.thermistor.ch0Adc, sensorData.thermistor.ch0Voltage, sensorData.thermistor.ch0Temp, sensorData.thermistor.ch0Resistance);
    thermister.readData(ThermisterChannel::CHANNEL_1, sensorData.thermistor.ch1Adc, sensorData.thermistor.ch1Voltage, sensorData.thermistor.ch1Temp, sensorData.thermistor.ch1Resistance);
}

void DataStore::setVoltageSensorData(VoltageSensor &voltageSensor)
{
    voltageSensor.read5V(&sensorData.voltage.fiveVolt);
    voltageSensor.read12V(&sensorData.voltage.twelveVolt);
}

void DataStore::setMolexPowerData(MolexPowerMeter &molexPowerMeter)
{
    molexPowerMeter.readPower(sensorData.molexPower);
}

void DataStore::loadConfigData()
{
    FANHUB_DEBUG_PRINTLN("Loading config data");
#ifdef ESP_EEPROM
    at24cx.readStruct(0, &configData, sizeof(configData));
#else
    ee.readBlock(0, (uint8_t *)&configData, sizeof(configData));
#endif
    FANHUB_DEBUG_PRINTLN("Config data loaded");
    serializedConfigDataDoc();
    serializeJsonPretty(configDataDoc, Serial);
    FANHUB_DEBUG_PRINTLN("");
}

void DataStore::setDefaultConfigData()
{
    sensorData.ram.free = 0;
    sensorData.ram.total = 0;
    sensorData.psram.free = 0;
    sensorData.psram.total = 0;

    sensorData.boardTemp.temp = 0.0;
    sensorData.boardTemp.humi = 0.0;
    sensorData.boardTemp.cpuTemp = 0.0;
    sensorData.thermistor.ch0Adc = 0.0;
    sensorData.thermistor.ch0Resistance = 0.0;
    sensorData.thermistor.ch0Voltage = 0.0;
    sensorData.thermistor.ch0Temp = 0.0;
    sensorData.thermistor.ch1Adc = 0.0;
    sensorData.thermistor.ch1Resistance = 0.0;
    sensorData.thermistor.ch1Voltage = 0.0;
    sensorData.thermistor.ch1Temp = 0.0;
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
    strncpy(configData.mdns, env_mdns, sizeof(configData.mdns));

    for (int i = 0; i < 5; i++)
    {
        configData.fanSource[i] = 0;
    }
    
    for (int i = 0; i < 5; i++)
    {
        configData.fanDuty[i] = 150;
    }

    configData.argb.mode = 0;
    configData.argb.speed = 1000;
    configData.argb.brightness = 255;
    configData.argb.source = LOW;
    configData.argb.color = 1179392;

    configData.modbus.unit_id = MODBUS_UNIT_ID;
    configData.modbus.baud_rate_index = MODBUS_BAUD_RATE_INDEX;

    for (int i = 0; i < 3; i++)
    {
        sensorData.molexPower.fiveVolt[i].current = 0.0;
        sensorData.molexPower.fiveVolt[i].voltage = 0.0;
        sensorData.molexPower.fiveVolt[i].power = 0.0;

        sensorData.molexPower.twelveVolt[i].current = 0.0;
        sensorData.molexPower.twelveVolt[i].voltage = 0.0;
        sensorData.molexPower.twelveVolt[i].power = 0.0;
    }
    sensorDataDoc["fw_version"] = FW_VERSION;
}

void DataStore::saveConfigData()
{
#ifdef ESP_EEPROM
    at24cx.writeStruct(0, &configData, sizeof(configData));
#else
    ee.setBlock(0, 0, sizeof(configData));
    ee.writeBlock(0, (uint8_t *)&configData, sizeof(configData));
#endif
    FANHUB_DEBUG_PRINTLN("Config data saved");
}

void DataStore::saveDefaultConfigData()
{
    setDefaultConfigData();
    saveConfigData();
}

void IRAM_ATTR DataStore::RESET_FLAG()
{
    // if (!instance)
    //     return;
    if (digitalRead(instance->resetPin) == LOW)
    {
        // instance->resetPressTime = millis();
        instance->resetFlag = true;
    }
    // else
    // {
    //     instance->resetFlag = false;
    // }
}

void DataStore::service()
{
    if (resetFlag)
    {
        FANHUB_DEBUG_PRINTLN("USER RESET");
        // unsigned long currentMillis = millis();
        // if (currentMillis - resetPressTime > resetThreshold)
        {
            saveDefaultConfigData();
            ESP.restart();
            // resetFlag = false;
        }
    }
}