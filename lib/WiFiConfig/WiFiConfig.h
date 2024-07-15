// lib/WiFiConfig/WifiConfig.h

#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <base64.h>
#include <Adafruit_NeoPixel.h>
#include <WS2812FX.h>
#include <ConfigData.h>
#include <ArduinoJson.h>
#include <SwitchSource.h>
#include <DataStore.h>
#include <FanControl.h>
#include <BaseModule.h>


class WiFiConfig : public BaseModule
{
    static WiFiConfig *instance;

private:
    IPAddress local_ip;
    IPAddress gateway;
    IPAddress subnet;
    IPAddress dns1;
    IPAddress dns2;
    void loadIpAddress();

public:
    AsyncWebServer server;
    AsyncWebSocket ws;
    DataStore *dataStore;
    WiFiConfig(DataStore *dataStore);
    bool verifyAuth(String password);
    void begin(WS2812FX &argb, SwitchSource &swSource, DataStore &dataStore, FanControl &fanControl);
    void begin(SwitchSource &swSource, DataStore &dataStore, FanControl &fanControl);
    static bool authHandler(AsyncWebServerRequest *request);
    void service();
};

#endif // WIFI_CONFIG_H
