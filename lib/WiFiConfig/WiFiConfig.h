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
#include <Update.h>
#include <Adafruit_NeoPixel.h>


#define ATTEMPT_CONNECT_DELAY 5000

class WiFiConfig : public BaseModule
{
    static WiFiConfig *instance;

private:
    IPAddress local_ip;
    IPAddress gateway;
    IPAddress subnet;
    IPAddress dns1;
    IPAddress dns2;
    uint16_t universe1 = 1; // 0 - 32767
    WS2812FX *ws2812;
    unsigned long lastReconnectAttempt = 0;
    void loadIpAddress();

public:
    AsyncWebServer server;
    AsyncWebSocket ws;
    DataStore *dataStore;
    WiFiConfig(DataStore *dataStore, WS2812FX *ws2812);
    bool verifyAuth(String password);
    void begin(WS2812FX &argb, SwitchSource &swSource, DataStore &dataStore, FanControl &fanControl);
    static bool authHandler(AsyncWebServerRequest *request);
    void service();
    int32_t rssi();
};

#endif // WIFI_CONFIG_H
