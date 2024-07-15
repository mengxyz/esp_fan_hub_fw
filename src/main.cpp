#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <ArtnetWifi.h>
#include <DataStore.h>
#include <FanControl.h>
#include <SwitchSource.h>
#include <Thermister.h>
#include <VoltageSensor.h>
#include <WS2812FX.h>
#include <WiFiConfig.h>
#include <MolexPowerMeter.h>
#include <BoardTempSensor.h>
#include <Oled.h>

#define BTN_UTIL_PIN_3 17 // ARGB RND

#define ARGB_PIN 8
#define ARGB_DEBUG_PIN 3
#define COLOR_ORDER GRB
#define CHIPSET WS2811
#define NUM_LEDS 40
#define BRIGHTNESS 200
#define FRAMES_PER_SECOND 60

int ARGB_BRIGHTNESS = 60;
int ARGB_MODE = 1;
bool ARGB_FILLED = false;
// WS2812FX ws2812fx = WS2812FX(1, ARGB_DEBUG_PIN, NEO_GBR + NEO_KHZ800);
WS2812FX ws2812fx = WS2812FX(NUM_LEDS, ARGB_PIN, NEO_GBR + NEO_KHZ800);

VoltageSensor voltageSensor;
MolexPowerMeter molexPowerMeter;
BoardTempSensor boardTempSensor;
Oled oled;

void initArgb()
{
}

Thermister thermister;
SwitchSource swSource;
DataStore dataStore;
FanControl fanControl;
WiFiConfig wifiConfig(&dataStore);

void IRAM_ATTR BTN_UTIL_3_ISR()
{
  
}

void initBtnUtility()
{
  pinMode(BTN_UTIL_PIN_3, INPUT_PULLUP);
  attachInterrupt(BTN_UTIL_PIN_3, BTN_UTIL_3_ISR, FALLING);
}

void loadArgbConfig()
{
  ws2812fx.init();
  ws2812fx.setSpeed(dataStore.configData.argb.speed);
  ws2812fx.setMode(dataStore.configData.argb.mode);
  ws2812fx.setBrightness(dataStore.configData.argb.brightness);
  ws2812fx.start();
}

void wifiTask(void *parameter)
{
  while (true)
  {
    wifiConfig.service();
    if (wifiConfig.ws.count() >= 1 && wifiConfig.ready())
    {
      // molexPowerMeter.readPower(dataStore.sensorData.molexPower);
      String out = dataStore.getSensorDataJson();
      wifiConfig.ws.textAll(out);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void mainLoopTask(void *parameter)
{
  unsigned long previousMillis = 0;
  const long interval = 1000;
  unsigned long currentMillis = 0;

  while (true)
  {
    // artnet.read();
    ws2812fx.service();
    currentMillis = millis();

    // if (setDefaultConfigData)
    // {
    //   dataStore.saveDefaultConfigData();
    //   Serial.println("Save default config data");
    //   delay(10);
    //   setDefaultConfigData = false;
    // }

    oled.service(dataStore.sensorData);

    if (currentMillis - previousMillis >= interval)
    {
      boardTempSensor.readSensors(dataStore.sensorData.boardTemp.temp, dataStore.sensorData.boardTemp.humi);
      dataStore.setThermisterData(thermister);
      swSource.readState(dataStore.configData.fanSource, dataStore.configData.argb.source);
      dataStore.setVoltageSensorData(voltageSensor);
      fanControl.readFanData(&dataStore.sensorData.fanData);
      previousMillis = currentMillis;
    }
    delay(10); // Add a small delay to avoid watchdog timer reset
  }
}

void fanTasks(void *parameter)
{
  for (;;)
  {
    fanControl.finalizePcnt();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  Serial.begin(115200);
  Wire.begin(12, 11);
  dataStore.begin();
  initArgb();
  loadArgbConfig();
  swSource.begin();
  swSource.initSource(dataStore.configData.fanSource, dataStore.configData.argb.source);
  fanControl.begin();
  fanControl.initAllDuty(dataStore.configData.fanDuty);
  initBtnUtility();
  voltageSensor.begin();
  thermister.begin();
  boardTempSensor.begin();
  wifiConfig.begin(ws2812fx, swSource, dataStore, fanControl);
  // molexPowerMeter.begin();
  oled.begin();

  // Create the wifi task and pin it to core 0
  xTaskCreatePinnedToCore(
      wifiTask,   // Task function
      "wifiTask", // Name of the task
      10000,      // Stack size (in bytes)
      NULL,       // Task input parameter
      1,          // Priority of the task
      NULL,       // Task handle
      0           // Core number (0 or 1)
  );

  // Create the main loop task and pin it to core 1
  xTaskCreatePinnedToCore(
      mainLoopTask,   // Task function
      "mainLoopTask", // Name of the task
      10000,          // Stack size (in bytes)
      NULL,           // Task input parameter
      1,              // Priority of the task
      NULL,           // Task handle
      1               // Core number (0 or 1)
  );

  xTaskCreatePinnedToCore(
      fanTasks,   // Task function
      "fanTasks", // Name of the task
      10000,      // Stack size (in bytes)
      NULL,       // Task input parameter
      2,          // Priority of the task
      NULL,       // Task handle
      1           // Core number (0 or 1)
  );              // Enable the alarm
}

void loop(){}
