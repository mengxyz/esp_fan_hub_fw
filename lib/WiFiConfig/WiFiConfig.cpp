// lib/WiFiConfig/WifiConfig.cpp
#include <WiFiConfig.h>

WiFiConfig *WiFiConfig::instance = nullptr;

void WiFiConfig::loadIpAddress()
{
   local_ip.fromString(this->dataStore->configData.local_ip);
   gateway.fromString(this->dataStore->configData.gateway);
   subnet.fromString(this->dataStore->configData.subnet);
   dns1.fromString(this->dataStore->configData.dns1);
   dns2.fromString(this->dataStore->configData.dns2);
}

bool WiFiConfig::verifyAuth(String password)
{
   String encodedPassword =
       base64::encode(String(this->dataStore->configData.auth_user) + ":" + String(this->dataStore->configData.auth_password));
   Serial.printf("PASSWORD HASH %s \n", encodedPassword);
   return password == encodedPassword;
}

bool WiFiConfig::authHandler(AsyncWebServerRequest *request)
{
   AsyncWebParameter *token = request->getParam("token");
   Serial.printf("User Try To Connect Using Token %s \n", token->value());
   if (instance != nullptr)
   {
      return instance->verifyAuth(token->value());
   }
   return false;
}

void WiFiConfig::service()
{
   unsigned long currentMillis = millis();
   if ((WiFi.status() != WL_CONNECTED) && this->isReady && (currentMillis - lastReconnectAttempt > ATTEMPT_CONNECT_DELAY))
   {
      this->isReady = false;
      WiFi.disconnect();
      WiFi.reconnect();
      lastReconnectAttempt = currentMillis;
      Serial.println("Reconnect WiFi");
   }
   else
   {
      ws.cleanupClients(4);
   }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
             AwsEventType type, void *arg, uint8_t *data, size_t len)
{
   switch (type)
   {
   case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(),
                    client->remoteIP().toString().c_str());
      break;
   case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
   case WS_EVT_PONG:
      Serial.printf("WebSocket Pong #%u \n", client->id());
      break;
   case WS_EVT_ERROR:
      Serial.printf("WebSocket Error #%u \n", client->id());
      break;
   }
}

WiFiConfig::WiFiConfig(DataStore *dataStore, WS2812FX *ws2812) : server(80), ws("/ws")
{
   this->dataStore = dataStore;
   this->ws2812 = ws2812;
   instance = this;
}

void authCheck(AsyncWebServerRequest *request, DataStore *dataStore, std::function<void()> handler)
{
   if (request->authenticate(dataStore->configData.auth_user, dataStore->configData.auth_password))
   {
      handler();
   }
   else
   {
      request->send(403, "text/json", "{\"message\":\"Authentication Failed\"}");
   }
}

void handleDoUpdate(AsyncWebServerRequest *request, DataStore *dataStore, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
{
   bool authenticated = request->authenticate(dataStore->configData.auth_user, dataStore->configData.auth_password);

   if (!index && authenticated)
   {
      Serial.println("Update");
      size_t content_len = request->contentLength();
      if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH))
      {
         Update.printError(Serial);
      }
   }

   if (authenticated)
   {
      if (Update.write(data, len) != len)
      {
         Update.printError(Serial);
      }
   }

   if (final && authenticated)
   {
      AsyncWebServerResponse *response = request->beginResponse(302, "text/json", "{\"message\":\"Please wait device restart..\"}");
      request->send(response);
      if (!Update.end(true))
      {
         Update.printError(Serial);
      }
      else
      {
         Serial.println("Update complete");
         Serial.flush();
         ESP.restart();
      }
   }
   else if (!authenticated)
   {
      AsyncWebServerResponse *response = request->beginResponse(403, "text/json", "{\"message\":\"Authentication Failed\"}");
      request->send(response);
   }
}

void printProgress(size_t prg, size_t sz)
{
   Serial.printf("Progress: %d%%\n", prg, sz);
}

int32_t WiFiConfig::rssi()
{
   return WiFi.RSSI();
}

void WiFiConfig::begin(WS2812FX &argb, SwitchSource &swSource, DataStore &dataStore, FanControl &fanControl)
{
   loadIpAddress();
   Serial.printf("WIFI_SSID %s\n", dataStore.configData.ssid);
   Serial.printf("WIFI_PASSWORD %s\n", dataStore.configData.password);
   Serial.printf("AUTH_USER %s\n", dataStore.configData.auth_user);
   Serial.printf("AUTH_PASSWORD %s\n", dataStore.configData.auth_password);

   WiFi.config(local_ip, gateway, subnet, dns1, dns2);
   WiFi.mode(WIFI_STA);
   WiFi.onEvent([&argb](WiFiEvent_t event, WiFiEventInfo_t info)
                {
      switch (event)
      {
      case SYSTEM_EVENT_STA_CONNECTED:
         Serial.println("Connected to access point");
         if (instance != nullptr) {
            Serial.printf("rssi %d dB\n", WiFi.RSSI());
            instance->isReady = true;
         }
         break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
         Serial.println("Disconnected from WiFi access point");
         break;
      case SYSTEM_EVENT_AP_STADISCONNECTED:
         Serial.println("WiFi client disconnected");
         break;
      default:
         break;
      } });
   WiFi.setTxPower(WIFI_POWER_19_5dBm);
   WiFi.begin(dataStore.configData.ssid, dataStore.configData.password);

   Serial.print("IP Address: ");
   Serial.println(WiFi.localIP());
   ws.setCustomHandler(authHandler);
   ws.onEvent(onEvent);
   server.addHandler(&ws);

   DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
   DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods",
                                        "GET, POST, PUT");
   DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers",
                                        "Content-Type");

   server.on("/auth", HTTP_GET, [](AsyncWebServerRequest *request)
             {
      if (!request->authenticate("user", "pass")) {
         return request->requestAuthentication();
      }
      request->send(200, "application/json",
                    "{\"message\":\"Login Successfully\"}"); });

   server.onNotFound([](AsyncWebServerRequest *request)
                     {
    if (request->method() == HTTP_OPTIONS) {
      request->send(204);
    } else {
      request->send(404);
    } });

   server.on("/mode", HTTP_GET, [&argb](AsyncWebServerRequest *request)
             {
      Serial.println("ON GET MODE");
      JsonDocument doc;
      doc["mode"] = argb.getMode();
      doc["max_speed"] = SPEED_MAX;
      doc["min_speed"] = SPEED_MIN;
      doc["speed"] = argb.getSpeed();
      doc["brightness"] = argb.getBrightness();
      doc["color"] = argb.getColor();
      String out;
      serializeJson(doc, out);
      request->send(200, "application/json", out); });

   server.on(
       "/mode", HTTP_POST, [](AsyncWebServerRequest *request) {},
       NULL, // We will not use the body handler here
       [&argb, &dataStore](AsyncWebServerRequest *request, uint8_t *data, size_t len,
                           size_t index, size_t total)
       {
          Serial.println("ON SET MODE");
          JsonDocument doc;
          deserializeJson(doc, data);
          // check exist mode
          if (doc.containsKey("speed"))
          {
             argb.setSpeed(doc["speed"]);
             dataStore.configData.argb.speed = doc["speed"];
          }
          if (doc.containsKey("color"))
          {
             String color = doc["color"];
             if (color.length() == 6)
             {
                uint32_t colorCode = strtoul(color.c_str(), NULL, 16);
                argb.setColor(colorCode);
             }
          }
          if (doc.containsKey("brightness"))
          {
             if (doc["brightness"] <= 255 && doc["brightness"] >= 0)
             {
                argb.setBrightness(doc["brightness"]);
                dataStore.configData.argb.brightness = doc["brightness"];
             }
          }
          if (doc.containsKey("mode"))
          {
             argb.setMode(doc["mode"]);
             dataStore.configData.argb.mode = doc["mode"];
          }
          dataStore.saveConfigData();
          request->send(204);
       });

   server.on(
       "/fan-source", HTTP_POST, [](AsyncWebServerRequest *request) {},
       NULL, // We will not use the body handler here
       [&swSource, &dataStore](AsyncWebServerRequest *request, uint8_t *data, size_t len,
                               size_t index, size_t total)
       {
          Serial.println("ON SET SOURCE");
          JsonDocument doc;
          deserializeJson(doc, data);

          // check exist mode
          if (doc.containsKey("ch") && doc.containsKey("source") && doc["source"] <= 5 && doc["source"] >= 0)
          {
             int ch = doc["ch"];
             if (ch == -1)
             {
                for (int i = 0; i < 5; i++)
                {
                   swSource.setSource(FAN_INPUT_SOURCES[i], doc["source"]);
                   dataStore.configData.fanSource[i] = doc["source"];
                }
                Serial.println("SET ALL SOURCE");
             }
             else
             {
                swSource.setSource(FAN_INPUT_SOURCES[ch], doc["source"]);
                dataStore.configData.fanSource[ch] = doc["source"];
             }
             dataStore.saveConfigData();
          }
          else
          {
             Serial.println("Invalid source");
          }
          request->send(204);
       });

   server.on(
       "/argb-source", HTTP_POST, [](AsyncWebServerRequest *request) {},
       NULL, // We will not use the body handler here
       [&swSource, &dataStore](AsyncWebServerRequest *request, uint8_t *data, size_t len,
                               size_t index, size_t total)
       {
          Serial.println("ON SET SOURCE");
          JsonDocument doc;
          deserializeJson(doc, data);

          // check exist mode
          if (doc["source"] <= 5 && doc["source"] >= 0)
          {
             swSource.setSource(InputSource::ARGB_SW, doc["source"]);
             dataStore.configData.argb.source = doc["source"];
             dataStore.saveConfigData();
          }
          else
          {
             Serial.println("Invalid Argb source");
          }
          request->send(204);
       });

   server.on(
       "/fan-duty", HTTP_POST, [](AsyncWebServerRequest *request) {},
       NULL, // We will not use the body handler here
       [&fanControl, &dataStore](AsyncWebServerRequest *request, uint8_t *data, size_t len,
                                 size_t index, size_t total)
       {
          Serial.println("ON SET SOURCE");
          JsonDocument doc;
          deserializeJson(doc, data);

          // check exist mode
          if (doc.containsKey("ch") && doc.containsKey("duty") && doc["duty"] <= 255 && doc["duty"] >= 0)
          {
             int ch = doc["ch"];
             if (ch == -1)
             {
                fanControl.setAllDuty(doc["duty"]);
                for (int i = 0; i < 5; i++)
                {
                   dataStore.configData.fanDuty[i] = doc["duty"];
                }
                Serial.println("SET ALL DUTY");
             }
             else
             {
                fanControl.setDuty(ch, doc["duty"]);
                dataStore.configData.fanDuty[ch] = doc["duty"];
             }
             dataStore.saveConfigData();
          }
          else
          {
             Serial.println("Invalid duty");
          }
          request->send(204);
       });

   server.on("/update", HTTP_POST, [](AsyncWebServerRequest *request) {}, [&dataStore](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
             { handleDoUpdate(request, &dataStore, filename, index, data, len, final); });

   server.on("/status", HTTP_GET, [&dataStore](AsyncWebServerRequest *request)
             { authCheck(request, &dataStore, [&]()
                         { request->send(200, "text/json", "{\"message\":\"OK\"}"); }); });
   server.on("/status", HTTP_POST, [](AsyncWebServerRequest *request) {}, [](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
             { request->send(200, "text/json", "{\"message\":\"OK\"}"); });
   server.begin();
   Update.onProgress(printProgress);
}