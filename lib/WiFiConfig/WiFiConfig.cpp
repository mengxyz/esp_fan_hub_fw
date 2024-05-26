// lib/WiFiConfig/WifiConfig.cpp

#include <WifiConfig.h>
#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <base64.h>
#include <Adafruit_NeoPixel.h>

#define ST(A) #A
#define STR(A) ST(A)

#ifdef WIFI_SSID
const char *ssid = STR(WIFI_SSID);
#endif

#ifdef WIFI_PASSWORD
const char *password = STR(WIFI_PASSWORD);
#endif

#ifdef AUTH_USER
const char *auth_user = STR(AUTH_USER);
#endif

#ifdef AUTH_PASSWORD
const char *auth_password = STR(AUTH_PASSWORD);
#endif

// const char* ssid = WIFI_SSID;
// const char* password = WIFI_PASSWORD;
// String AUTH_USER = FLAG_AUTH_USER;
// String AUTH_PASSWORD = FLAG_AUTH_PASSWORD;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

IPAddress local_IP(192, 168, 1, 184);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

bool verifyAuth(String cpassword)
{
    String encodedPassword = base64::encode(String(auth_user) + ":" + String(auth_password));
    Serial.printf("PASSWORD HASH %s \n", encodedPassword);
    return cpassword == encodedPassword;
}

bool authHandler(AsyncWebServerRequest *request)
{
    AsyncWebParameter *token = request->getParam("token");
    Serial.printf("User Try To Connect Using Token %s \n", token->value());
    return verifyAuth(token->value());
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    data[len] = 0;
    String sdata = String((char *)data);
    String targetPrefix = sdata.substring(0, 1);

    // Uncomment and implement other handlers if needed
    // if (targetPrefix == "D")
    // {
    //     handlerDuty(sdata);
    //     return;
    // }
    if (targetPrefix == "C")
    {
        // handlerArgb(sdata);
        return;
    }
    // if (targetPrefix == "P")
    // {
    //     handlePcf8574(sdata);
    //     return;
    // }
    // if (targetPrefix == "B")
    // {
    //     handleArgbBrightness(sdata);
    //     return;
    // }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
    case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len);
        break;
    case WS_EVT_PONG:
        Serial.printf("WebSocket Pong #%u \n", client->id());
        break;
    case WS_EVT_ERROR:
        Serial.printf("WebSocket Error #%u \n", client->id());
        break;
    }
}

void initWifi(Adafruit_NeoPixel pixels)
{
    Serial.printf("WIFI_SSID %s\n", ssid);
    Serial.printf("WIFI_PASSWORD %s\n", password);
    Serial.printf("AUTH_USER %s\n", auth_user);
    Serial.printf("AUTH_PASSWORD %s\n", auth_password);

    WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    // set pixel blue color and show on pixel 1
    pixels.setPixelColor(0, 0, 0, 255);
    pixels.show();

    if (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.printf("WiFi Failed!\n");
        pixels.setPixelColor(0, 255, 0, 0);
        return;
    }

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    ws.setCustomHandler(authHandler);
    ws.onEvent(onEvent);
    server.addHandler(&ws);

    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Accept, Content-Type, Authorization");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Credentials", "true");

    server.on("/auth", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  if (!request->authenticate("user", "pass"))
                  {
                      return request->requestAuthentication();
                  }
                  request->send(200, "application/json", "{\"message\":\"Login Successfully\"}"); });

    server.on("/auth", HTTP_OPTIONS, [](AsyncWebServerRequest *request)
              { request->send(200); });

    server.begin();
    pixels.setPixelColor(0, 0, 255, 0);
    pixels.show();
}