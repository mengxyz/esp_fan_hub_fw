// lib/WiFiConfig/WifiConfig.cpp
#include <RTClib.h>
#include <WS2812FX.h>
#include <WiFiConfig.h>

WiFiConfig *WiFiConfig::instance = nullptr;

void WiFiConfig::loadIpAddress() {
  local_ip.fromString(this->dataStore->configData.local_ip);
  gateway.fromString(this->dataStore->configData.gateway);
  subnet.fromString(this->dataStore->configData.subnet);
  dns1.fromString(this->dataStore->configData.dns1);
  dns2.fromString(this->dataStore->configData.dns2);
}

// void WiFiConfig::configWifi() {
//   loadIpAddress();
//   FANHUB_DEBUG_PRINTF("WIFI_SSID %s\n", dataStore->configData.ssid);
//   FANHUB_DEBUG_PRINTF("WIFI_PASSWORD %s\n", dataStore->configData.password);
//   FANHUB_DEBUG_PRINTF("AUTH_USER %s\n", dataStore->configData.auth_user);
//   FANHUB_DEBUG_PRINTF("AUTH_PASSWORD %s\n", dataStore->configData.auth_password);
//   FANHUB_DEBUG_PRINTF("MDNS %s\n", dataStore->configData.mdns);

//   WiFi.config(local_ip, gateway, subnet, dns1, dns2);
//   WiFi.mode(WIFI_STA);
//   WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
//     switch (event) {
//     case SYSTEM_EVENT_STA_CONNECTED:
//       FANHUB_DEBUG_PRINTLN("Connected to access point");
//       if (instance != nullptr) {
//         FANHUB_DEBUG_PRINTF("rssi %d dB\n", WiFi.RSSI());
//         instance->isReady = true;
//         instance->startServer();
//         // set green color
//         if (instance->argbStatus != nullptr) {
//           instance->argbStatus->setPixelColor(0, 0, 0, 255);
//           instance->argbStatus->show();
//         } else {
//           FANHUB_DEBUG_PRINTLN("argbStatus is null");
//         }
//       }
//       break;
//     case SYSTEM_EVENT_STA_DISCONNECTED:
//       FANHUB_DEBUG_PRINTLN("Disconnected from WiFi access point");
//       // set red color
//       if (instance->argbStatus != nullptr) {
//         instance->argbStatus->setPixelColor(0, 255, 0, 0);
//         instance->argbStatus->show();
//       } else {
//         FANHUB_DEBUG_PRINTLN("argbStatus is null");
//       }
//       break;
//     case SYSTEM_EVENT_AP_STADISCONNECTED:
//       FANHUB_DEBUG_PRINTLN("WiFi client disconnected");
//       break;
//     default:
//       break;
//     }
//   });
//   WiFi.setTxPower(WIFI_POWER_19_5dBm);
//   WiFi.begin(instance->dataStore->configData.ssid,
//              instance->dataStore->configData.password);
// }

void WiFiConfig::configWifi() {
  loadIpAddress();

  // Debugging information
  FANHUB_DEBUG_PRINTF("WIFI_SSID %s\n", dataStore->configData.ssid);
  FANHUB_DEBUG_PRINTF("WIFI_PASSWORD %s\n", dataStore->configData.password);
  FANHUB_DEBUG_PRINTF("AUTH_USER %s\n", dataStore->configData.auth_user);
  FANHUB_DEBUG_PRINTF("AUTH_PASSWORD %s\n", dataStore->configData.auth_password);
  FANHUB_DEBUG_PRINTF("MDNS %s\n", dataStore->configData.mdns);

#ifdef FANHUB_AP_MODE
  // AP Mode Configuration
  FANHUB_DEBUG_PRINTLN("Starting in AP Mode");
  WiFi.mode(WIFI_AP);               // Set WiFi to Access Point mode
  const char *apSSID = "FanHub_AP"; // Default AP SSID
  WiFi.softAP(apSSID);              // Start AP without a password
  FANHUB_DEBUG_PRINTF("AP SSID: %s\n", apSSID);

  // Optional: Set a custom IP for the AP
  WiFi.softAPConfig(local_ip, gateway, subnet);

  // Indicate AP mode with a specific color (e.g., blue)
  if (instance->argbStatus != nullptr) {
    instance->argbStatus->setPixelColor(0, 0, 0, 255); // Blue color
    instance->argbStatus->show();
  } else {
    FANHUB_DEBUG_PRINTLN("argbStatus is null");
  }

#else
  WiFi.config(local_ip, gateway, subnet, dns1, dns2);
  // STA Mode Configuration
  FANHUB_DEBUG_PRINTLN("Starting in STA Mode");
  WiFi.mode(WIFI_STA); // Set WiFi to Station mode

  WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
    switch (event) {
    case SYSTEM_EVENT_STA_CONNECTED:
      FANHUB_DEBUG_PRINTLN("Connected to access point");
      if (instance != nullptr) {
        FANHUB_DEBUG_PRINTF("rssi %d dB\n", WiFi.RSSI());
        instance->isReady = true;
        instance->startServer();
        // Set green color
        if (instance->argbStatus != nullptr) {
          instance->argbStatus->setPixelColor(0, 0, 255, 0); // Green color
          instance->argbStatus->show();
        } else {
          FANHUB_DEBUG_PRINTLN("argbStatus is null");
        }
      }
      break;

    case SYSTEM_EVENT_STA_DISCONNECTED:
      FANHUB_DEBUG_PRINTLN("Disconnected from WiFi access point");
      // Set red color
      if (instance->argbStatus != nullptr) {
        instance->argbStatus->setPixelColor(0, 255, 0, 0); // Red color
        instance->argbStatus->show();
      } else {
        FANHUB_DEBUG_PRINTLN("argbStatus is null");
      }
      break;

    case SYSTEM_EVENT_AP_STADISCONNECTED:
      FANHUB_DEBUG_PRINTLN("WiFi client disconnected");
      break;

    default:
      break;
    }
  });

  WiFi.setTxPower(WIFI_POWER_19_5dBm);
  WiFi.begin(instance->dataStore->configData.ssid,
             instance->dataStore->configData.password);
#endif
}

void WiFiConfig::startServer() {
  FANHUB_DEBUG_PRINT("IP Address: ");
  FANHUB_DEBUG_PRINTLN(WiFi.localIP());

  if (!MDNS.begin("espfanhub")) {
    FANHUB_DEBUG_PRINTLN("Error starting mDNS");
  }
  MDNS.addService("http", "tcp", 80);

  // server.config.max_uri_handlers = 20;
  // ws.setCustomHandler(authHandler);
  // ws.onEvent(onEvent);
  // server.addHandler(&ws);

  // DefaultHeaders::Instance().addHeader("Server", "PsychicHttp");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods",
                                       "GET, POST, PUT, DELETE, OPTIONS");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers",
                                       "Content-Type, Authorization, Accept");
  DefaultHeaders::Instance().addHeader("Access-Control-Max-Age", "86400");

  server.listen(80);

  server.on("/config", HTTP_GET, [](PsychicRequest *request) {
    JsonDocument doc = instance->dataStore->getConfigDataDoc();
    uint8_t modeCount = instance->argbStrip->getModeCount();
    for (uint8_t i = 0; i < modeCount; i++) {
      doc["argb"]["modes"][i] = instance->argbStrip->getModeName(i);
    }
    String output;
    serializeJson(doc, output);
    return request->reply(200, "application/json", output.c_str());
  });

  server.on("/reset", HTTP_GET, [](PsychicRequest *request) {
    instance->resetFlag = true;
    return request->reply(
        200, "application/json",
        "{\"message\": \"Device will be reset in a few seconds\"}");
  });

  server.on("/restart", HTTP_GET, [](PsychicRequest *request) {
    instance->restartFlag = true;
    return request->reply(
        200, "application/json",
        "{\"message\": \"Device will be restarted in a few seconds\"}");
  });

  server.on("/sync-time", HTTP_GET, [](PsychicRequest *request) {
    if (instance->rtcUpdateCallback != nullptr) {
      instance->rtcUpdateCallback();
    }
    return request->reply(204);
  });

  server.on("/mode", HTTP_GET, [](PsychicRequest *request) {
    FANHUB_DEBUG_PRINTLN("ON GET MODE");
    JsonDocument doc;
    uint8_t modeCount = instance->argbStrip->getModeCount();
    for (uint8_t i = 0; i < modeCount; i++) {
      doc["modes"][i] = instance->argbStrip->getModeName(i);
    }
    doc["mode"] = instance->argbStrip->getMode();
    doc["max_speed"] = SPEED_MAX;
    doc["min_speed"] = SPEED_MIN;
    doc["speed"] = instance->argbStrip->getSpeed();
    doc["brightness"] = instance->argbStrip->getBrightness();
    doc["color"] = instance->argbStrip->getColor();
    String out;
    serializeJson(doc, out);
    return request->reply(200, "application/json", out.c_str());
  });

  server.on("/mode", HTTP_POST, [](PsychicRequest *request) {
    FANHUB_DEBUG_PRINTLN("ON SET MODE");
    JsonDocument doc;
    deserializeJson(doc, request->body());
    // check exist mode
    if (doc["speed"].is<uint8_t>()) {
      instance->argbStrip->setSpeed(doc["speed"].as<uint8_t>());
      instance->dataStore->configData.argb.speed = doc["speed"].as<uint8_t>();
    }
    if (doc["speed"].is<String>()) {
      String color = doc["color"].as<String>();
      if (color.length() == 6) {
        uint32_t colorCode = strtoul(color.c_str(), NULL, 16);
        instance->argbStrip->setColor(colorCode);
        instance->dataStore->configData.argb.color = colorCode;
      }
    }
    if (doc["brightness"].is<uint8_t>()) {
      if (doc["brightness"].as<uint8_t>() <= 255 &&
          doc["brightness"].as<uint8_t>() >= 0) {
        instance->argbStrip->setBrightness(doc["brightness"].as<uint8_t>());
        instance->dataStore->configData.argb.brightness =
            doc["brightness"].as<uint8_t>();
      }
    }
    if (doc["mode"].is<uint8_t>()) {
      instance->argbStrip->setMode(doc["mode"].as<uint8_t>());
      instance->dataStore->configData.argb.mode = doc["mode"].as<uint8_t>();
    }
    instance->dataStore->saveConfigData();
    return request->reply(204);
  });

  server.on("/fan-source", HTTP_POST, [](PsychicRequest *request) {
    FANHUB_DEBUG_PRINTLN("ON SET SOURCE");
    JsonDocument doc;
    deserializeJson(doc, request->body());

    if (!doc["ch"].is<uint8_t>()) {
      FANHUB_DEBUG_PRINTLN("Invalid ch");
      return request->reply(400);
    }

    if (!doc["source"].is<bool>() || doc["ch"].as<uint8_t>() > 5 ||
        doc["ch"].as<uint8_t>() < 0) {
      FANHUB_DEBUG_PRINTLN("Invalid source");
      return request->reply(400);
    }

    uint8_t ch = doc["ch"].as<uint8_t>();
    uint8_t source = doc["source"].as<bool>();

    if (ch == -1) {
      for (int i = 0; i < 5; i++) {
        instance->swSource->setSource(FAN_INPUT_SOURCES[i], source);
        instance->dataStore->configData.fanSource[i] = source;
      }
      FANHUB_DEBUG_PRINTLN("SET ALL SOURCE");
    } else {
      instance->swSource->setSource(FAN_INPUT_SOURCES[ch], source);
      instance->dataStore->configData.fanSource[ch] = source;
    }
    instance->dataStore->saveConfigData();
    return request->reply(204);
  });

  server.on("/argb-source", HTTP_POST, [](PsychicRequest *request) {
    FANHUB_DEBUG_PRINTLN("ON SET SOURCE");
    JsonDocument doc;
    deserializeJson(doc, request->body());

    if (!doc["source"].is<bool>()) {
      FANHUB_DEBUG_PRINTLN("Invalid source");
      return request->reply(400);
    }

    bool source = doc["source"].as<bool>();

    // check exist mode
    instance->swSource->setSource(InputSource::ARGB_SW, source);
    instance->dataStore->configData.argb.source = source;
    instance->dataStore->saveConfigData();
    return request->reply(204);
  });

  server.on("/fan-duty", HTTP_POST, [](PsychicRequest *request) {
    FANHUB_DEBUG_PRINTLN("ON SET DUTY");
    JsonDocument doc;
    deserializeJson(doc, request->body());

    if (!doc["ch"].is<uint8_t>()) {
      FANHUB_DEBUG_PRINTLN("Invalid ch");
      return request->reply(400);
    }
    if (!doc["duty"].is<uint8_t>()) {
      FANHUB_DEBUG_PRINTLN("Invalid duty");
      return request->reply(400);
    }
    if (doc["duty"].as<uint8_t>() > 255 || doc["duty"].as<uint8_t>() < 0) {
      FANHUB_DEBUG_PRINTLN("Invalid duty");
      return request->reply(400);
    }

    uint8_t ch = doc["ch"].as<uint8_t>();
    uint8_t duty = doc["duty"].as<uint8_t>();

    if (ch == -1) {
      instance->fanControl->setAllDuty(duty);
      for (int i = 0; i < 5; i++) {
        instance->dataStore->configData.fanDuty[i] = duty;
      }
      FANHUB_DEBUG_PRINTLN("SET ALL DUTY");
    } else {
      FANHUB_DEBUG_PRINTF("SET DUTY %d = %d \n", ch, duty);
      instance->fanControl->setDuty(ch, duty);
      instance->dataStore->configData.fanDuty[ch] = duty;
    }
    instance->dataStore->saveConfigData();
    return request->reply(204);
  });

  updateHandler->onUpload(handleUpdate);
  server.on("/update", HTTP_POST, updateHandler);

  server.on("/status", HTTP_GET, [](PsychicRequest *request) {
    PsychicJsonResponse response = PsychicJsonResponse(request);

    JsonObject root = response.getRoot();
    root["message"] = "Pong";
    root["millis"] = millis();
    return response.send();
  });

  ws.onOpen([](PsychicWebSocketClient *client) {
    FANHUB_DEBUG_PRINTF("[socket] connection #%u connected from %s\n",
                 client->socket(), client->remoteIP().toString());
    client->sendMessage("Hello!");
  });
  ws.onFrame([](PsychicWebSocketRequest *request, httpd_ws_frame *frame) {
    FANHUB_DEBUG_PRINTF("[socket] #%d sent: %s\n", request->client()->socket(),
                 (char *)frame->payload);
    return request->reply(frame);
  });
  ws.onClose([](PsychicWebSocketClient *client) {
    FANHUB_DEBUG_PRINTF("[socket] connection #%u closed from %s\n", client->socket(),
                 client->remoteIP().toString());
  });

  server.on("/ws", &ws);

  server.on("*", HTTP_OPTIONS,
            [](PsychicRequest *request) { return request->reply(204); });
}

bool WiFiConfig::verifyAuth(String password) {
  String encodedPassword =
      base64::encode(String(this->dataStore->configData.auth_user) + ":" +
                     String(this->dataStore->configData.auth_password));
  FANHUB_DEBUG_PRINTF("PASSWORD HASH %s \n", encodedPassword);
  return password == encodedPassword;
}

// bool WiFiConfig::authHandler(AsyncWebServerRequest *request)
// {
//    AsyncWebParameter *token = request->getParam("token");
//    FANHUB_DEBUG_PRINTF("User Try To Connect Using Token %s \n", token->value());
//    if (instance != nullptr)
//    {
//       return instance->verifyAuth(token->value());
//    }
//    return false;
// }

void WiFiConfig::broadcastSensorData() {
  unsigned long currentMillis = millis();
  if (ws.count() >= 1 && ready() && currentMillis - lastBroadCast > 1000) {
    String out = dataStore->getSensorDataJson();
    ws.sendAll(out.c_str());
    lastBroadCast = currentMillis;
  }
}

void WiFiConfig::service() {
  unsigned long currentMillis = millis();
  if ((WiFi.status() != WL_CONNECTED) && this->isReady &&
      (currentMillis - lastReconnectAttempt > ATTEMPT_CONNECT_DELAY)) {
    this->isReady = false;
    WiFi.disconnect();
    WiFi.reconnect();
    lastReconnectAttempt = currentMillis;
    FANHUB_DEBUG_PRINTLN("Reconnect WiFi");
  } else {
    // ws.cleanupClients(4);
    // TODO
  }

  if (this->resetFlag) {
    instance->dataStore->saveDefaultConfigData();
    ESP.restart();
  }

  if (this->restartFlag) {
    ESP.restart();
  }
}

// void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
//              AwsEventType type, void *arg, uint8_t *data, size_t len)
// {
//    switch (type)
//    {
//    case WS_EVT_CONNECT:
//       FANHUB_DEBUG_PRINTF("WebSocket client #%u connected from %s\n", client->id(),
//                     client->remoteIP().toString().c_str());
//       break;
//    case WS_EVT_DISCONNECT:
//       FANHUB_DEBUG_PRINTF("WebSocket client #%u disconnected\n", client->id());
//       break;
//    case WS_EVT_PONG:
//       FANHUB_DEBUG_PRINTF("WebSocket Pong #%u \n", client->id());
//       break;
//    case WS_EVT_ERROR:
//       FANHUB_DEBUG_PRINTF("WebSocket Error #%u \n", client->id());
//       break;
//    }
// }

WiFiConfig::WiFiConfig(DataStore *dataStore, SwitchSource *swSource,
                       FanControl *fanControl) {
  this->dataStore = dataStore;
  this->swSource = swSource;
  this->fanControl = fanControl;
  instance = this;
}

// void authCheck(AsyncWebServerRequest *request, DataStore *dataStore,
// std::function<void()> handler)
// {
//    if (request->authenticate(dataStore->configData.auth_user,
//    dataStore->configData.auth_password))
//    {
//       handler();
//    }
//    else
//    {
//       request->send(403, "text/json", "{\"message\":\"Authentication
//       Failed\"}");
//    }
// }

esp_err_t WiFiConfig::handleUpdate(PsychicRequest *request,
                                   const String &filename, uint64_t index,
                                   uint8_t *data, size_t len, bool last) {
  bool authenticated =
      request->authenticate(instance->dataStore->configData.auth_user,
                            instance->dataStore->configData.auth_password);

  if (!index && authenticated) {
    FANHUB_DEBUG_PRINTLN("Update");
    size_t content_len = request->contentLength();
    if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH)) {
      Update.printError(Serial);
      return ESP_FAIL;
    }
  }

  if (authenticated) {
    if (Update.write(data, len) != len) {
      Update.printError(Serial);
      return ESP_FAIL;
    }

    // Calculate and print progress
    size_t content_len = request->contentLength();
    float progress = (float)(index + len) / content_len * 100;
    FANHUB_DEBUG_PRINTF("Progress: %.2f%%\n", progress);
    instance->onUpdateCallback(progress);
  }

  if (last && authenticated) {
    PsychicJsonResponse response = PsychicJsonResponse(request);
    JsonObject root = response.getRoot();
    root["message"] = "Please wait device restart..";
    response.send();
    if (!Update.end(true)) {
      Update.printError(Serial);
      return ESP_FAIL;
    } else {
      FANHUB_DEBUG_PRINTLN("Update complete");
      Serial.flush();
      ESP.restart();
    }
  } else if (!authenticated) {
    PsychicResponse response = PsychicResponse(request);
    response.setCode(403);
    response.setContentType("text/json");
    response.setContent("{\"message\":\"Authentication Failed\"}");
    response.send();
    return ESP_FAIL;
  }
  return ESP_OK;
}

void printProgress(size_t prg, size_t sz) {
  FANHUB_DEBUG_PRINTF("Progress: %d%%\n", prg, sz);
}

int32_t WiFiConfig::rssi() { return WiFi.RSSI(); }

void WiFiConfig::begin() { configWifi(); }