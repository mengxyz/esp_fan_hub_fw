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
   if ((WiFi.status() != WL_CONNECTED) && this->isReady)
   {
      this->isReady = false;
      WiFi.disconnect();
      WiFi.reconnect();
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

WiFiConfig::WiFiConfig(DataStore *dataStore) : server(80), ws("/ws")
{
   this->dataStore = dataStore;
   instance = this;
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
      Serial.printf("WiFi event %d\n", event);
      switch (event)
      {
      case SYSTEM_EVENT_STA_CONNECTED:
         Serial.println("Connected to access point");
         if (instance != nullptr) {
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

   server.on("/auth", HTTP_OPTIONS,
             [](AsyncWebServerRequest *request)
             { request->send(200); });

   server.on("/mode", HTTP_OPTIONS,
             [](AsyncWebServerRequest *request)
             { request->send(200); });

   server.on("/fan-source", HTTP_OPTIONS,
             [](AsyncWebServerRequest *request)
             { request->send(200); });

   server.on("/fan-duty", HTTP_OPTIONS,
             [](AsyncWebServerRequest *request)
             { request->send(200); });

   server.on("/argb-source", HTTP_OPTIONS,
             [](AsyncWebServerRequest *request)
             { request->send(200); });
             
   server.on("/restart", HTTP_OPTIONS,
             [](AsyncWebServerRequest *request)
             { request->send(200); });
             
   server.on("/restart", HTTP_GET,
             [](AsyncWebServerRequest *request)
             {
                ESP.restart();
             });

   server.on("/mode", HTTP_GET, [&argb](AsyncWebServerRequest *request)
             {
      Serial.println("ON GET MODE");
      JsonDocument doc;
      doc["mode"] = argb.getMode();
      doc["max_speed"] = SPEED_MAX;
      doc["min_speed"] = SPEED_MIN;
      doc["speed"] = argb.getSpeed();
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
   server.begin();
}

void WiFiConfig::begin(SwitchSource &swSource, DataStore &dataStore, FanControl &fanControl)
{
   loadIpAddress();
   Serial.printf("WIFI_SSID %s\n", dataStore.configData.ssid);
   Serial.printf("WIFI_PASSWORD %s\n", dataStore.configData.password);
   Serial.printf("AUTH_USER %s\n", dataStore.configData.auth_user);
   Serial.printf("AUTH_PASSWORD %s\n", dataStore.configData.auth_password);

   WiFi.config(local_ip, gateway, subnet, dns1, dns2);
   WiFi.mode(WIFI_STA);
   WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info)
                {
      Serial.printf("WiFi event %d\n", event);
      switch (event)
      {
      case SYSTEM_EVENT_STA_CONNECTED:
         Serial.println("Connected to access point");
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

   server.on("/auth", HTTP_OPTIONS,
             [](AsyncWebServerRequest *request)
             { request->send(200); });

   server.on("/mode", HTTP_OPTIONS,
             [](AsyncWebServerRequest *request)
             { request->send(200); });

   server.on("/fan-source", HTTP_OPTIONS,
             [](AsyncWebServerRequest *request)
             { request->send(200); });

   server.on("/fan-duty", HTTP_OPTIONS,
             [](AsyncWebServerRequest *request)
             { request->send(200); });

   server.on("/argb-source", HTTP_OPTIONS,
             [](AsyncWebServerRequest *request)
             { request->send(200); });

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
   server.begin();
}