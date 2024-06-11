#include <Arduino.h>
#include <Sht40.h>
#include <Wire.h>
// #include <Adafruit_NeoPixel.h>
#include <DataStore.h>
#include <FanControl.h>
#include <SwitchSource.h>
#include <Thermister.h>
#include <VoltageSensor.h>
#include <WS2812FX.h>
#include <WiFiConfig.h>

#define BTN_UTIL_PIN_3 17 // ARGB RND

#define ARGB_PIN 8
#define COLOR_ORDER GRB
#define CHIPSET WS2811
#define NUM_LEDS 40
#define BRIGHTNESS 200
#define FRAMES_PER_SECOND 60

int ARGB_BRIGHTNESS = 60;
int ARGB_MODE = 1;
bool ARGB_FILLED = false;
// Adafruit_NeoPixel pixels(NUM_LEDS, ARGB_PIN, NEO_GRB + NEO_KHZ800);
WS2812FX ws2812fx = WS2812FX(NUM_LEDS, ARGB_PIN, NEO_RGB + NEO_KHZ800);
// create static varables store a colors for test named argbTestColors
uint32_t argbTestColors[8] = {
    0xFF000000,
    0xFF0000FF,
    0xFF00FF00,
    0xFF00FFFF,
    0xFFFFFF00,
    0xFFFF0000,
    0xFFFF00FF,
    0xFFFFFF,
};

VoltageSensor voltageSensor;

void initArgb()
{
  // pixels.setBrightness(ARGB_BRIGHTNESS);
  // pixels.clear();
  // pixels.show();

  ws2812fx.init();
  ws2812fx.setBrightness(ARGB_BRIGHTNESS);
  ws2812fx.setSpeed(1000);
  ws2812fx.setColor(0x007BFF);
  ws2812fx.setMode(FX_MODE_STATIC);
  ws2812fx.start();
}

Thermister thermister;
SwitchSource swSource;
DataStore dataStore;
FanControl fanControl;
WiFiConfig wifiConfig(&dataStore);

int testColorsIndex = 0;
bool testColorsIndexChanged = false;
// loop the colors
void IRAM_ATTR BTN_UTIL_3_ISR()
{
  testColorsIndex++;
  if (testColorsIndex > 7)
  {
    testColorsIndex = 0;
  }
  testColorsIndexChanged = true;
}

void initBtnUtility()
{
  pinMode(BTN_UTIL_PIN_3, INPUT_PULLUP);
  attachInterrupt(BTN_UTIL_PIN_3, BTN_UTIL_3_ISR, FALLING);
}

void loadArgbConfig() {
  ws2812fx.setSpeed(dataStore.configData.argb.speed);
  ws2812fx.setMode(dataStore.configData.argb.mode);
  ws2812fx.setBrightness(dataStore.configData.argb.brightness);
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
  fanControl.beginBtnUtils();
  initBtnUtility();
  voltageSensor.begin();
  thermister.begin();
  initSht40();
  // initWifi(pixels);
  wifiConfig.begin(ws2812fx, swSource, dataStore, fanControl);
}

unsigned long previousMillis = 0;
const long interval = 1000;

#define TIMER_MS 5000
unsigned long last_change = 0;

void loop()
{
  unsigned long currentMillis = millis();

  ws2812fx.service();

  if (currentMillis - previousMillis >= interval)
  {
    wifiConfig.ws.cleanupClients(4);
    if (wifiConfig.ws.count() >= 1)
    {
      dataStore.setSht40Data(sht);
      dataStore.setThermisterData(thermister);
      dataStore.setVoltageSensorData(voltageSensor);
      swSource.readState(dataStore.configData.fanSource, dataStore.configData.argb.source);
      dataStore.setFanData(fanControl);
      // dataStore.printSensorData();
      String out = dataStore.getSensorDataJson();
      wifiConfig.ws.textAll(out);
      Serial.println("Data sent");
    }
    previousMillis = currentMillis;
    fanControl.resetFreqs();
  }
}
