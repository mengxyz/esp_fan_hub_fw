// lib/WiFiConfig/WifiConfig.h

#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <base64.h>
#include <Adafruit_NeoPixel.h>

typedef void (*WiFiCallback)();
typedef void (*WiFiFailCallback)();

// Web server and WebSocket
extern AsyncWebServer server;
extern AsyncWebSocket ws;

// IP address configuration
extern IPAddress local_IP;
extern IPAddress gateway;
extern IPAddress subnet;
extern IPAddress primaryDNS;
extern IPAddress secondaryDNS;

// Function declarations
void initWifi(Adafruit_NeoPixel pixels);
bool verifyAuth(String cpassword);
bool authHandler(AsyncWebServerRequest *request);
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

#endif // WIFI_CONFIG_H
