// lib/WiFiConfig/WifiConfig.h

#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include <Arduino.h>
#include <WiFi.h>
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
#include <PsychicHttp.h>
#include <CustomPsychicWebSocketHandler.h>
#include <ESPmDNS.h>


#define ATTEMPT_CONNECT_DELAY 5000

typedef void (*RtcUpdateCallback)();
typedef void (*OnUpdateCallback)(float progress);

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
    WS2812FX *argbStatus = nullptr;
    WS2812FX *argbStrip;
    RtcUpdateCallback rtcUpdateCallback = nullptr;
    OnUpdateCallback onUpdateCallback = nullptr;
    bool restartFlag = false;
    bool resetFlag = false;
    volatile unsigned long lastReconnectAttempt = 0;
    volatile unsigned long lastBroadCast = 0;
    void loadIpAddress();
    void configWifi();
    void startServer();

public:
    PsychicHttpServer server;
    PsychicWebSocketHandler ws;
    PsychicUploadHandler *updateHandler = new PsychicUploadHandler();
    DataStore *dataStore;
    SwitchSource *swSource;
    FanControl *fanControl;
    WiFiConfig(DataStore *dataStore, SwitchSource *swSource, FanControl *fanControl);
    bool verifyAuth(String password);
    static esp_err_t handleUpdate(PsychicRequest *request, const String &filename, uint64_t index, uint8_t *data, size_t len, bool last);
    void begin();
    // static bool authHandler(AsyncWebServerRequest *request);
    void broadcastSensorData();
    void service();
    int32_t rssi();
    void useLedStatus(uint8_t pin, uint8_t count = 1, uint8_t brightness = 255) {
        argbStatus = new WS2812FX(count, pin, NEO_GRB + NEO_KHZ800);
    }

    void useArgbStrip(WS2812FX &strip) {
        argbStrip = &strip;
    }

    void setRtcUpdateCallback(RtcUpdateCallback callback) { // Corrected parameter type
        rtcUpdateCallback = callback;
    }

    void setOnUpdateCallback(OnUpdateCallback callback) { // Corrected parameter type
        onUpdateCallback = callback;
    }
};

#endif // WIFI_CONFIG_H
