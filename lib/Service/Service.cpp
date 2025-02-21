#include <Service.h>
#define ACTIVE_BUZZER_PIN 20
int ARGB_BRIGHTNESS = 60;
int ARGB_MODE = 1;
bool ARGB_FILLED = false;
WS2812FX ws2812fx = WS2812FX(NUM_LEDS, ARGB_PIN, NEO_GRB + NEO_KHZ800);
// WS2812FX ws2812fx = WS2812FX(NUM_LEDS, ARGB_DEBUG_PIN, NEO_GRB + NEO_KHZ800);

/// Define Sensors
VoltageSensor voltageSensor(INA219_5V_I2C_ADDRESS, INA219_12V_I2C_ADDRESS);
MolexPowerMeter molexPowerMeter;
BoardTempSensor boardTempSensor(SHTSensor::AUTO_DETECT);
Oled oled;
Thermister thermister;
SwitchSource swSource(PCF8574_I2C_ADDRESS);
DataStore dataStore(EEPROM_I2C_ADDRESS, I2C_DEVICESIZE_24LC256,
                    EEPROM_RESET_PIN);
FanControl fanControl;
WiFiConfig wifiConfig(&dataStore, &swSource, &fanControl);
#ifdef USE_RTC_DS3231
RTC_DS3231 rtc;
#endif
#ifdef USE_RTC_PCF8563
RTC_PCF8563 rtc;
#endif
// ActionButton actionBtn(EEPROM_RESET_PIN, 2, 20);

#ifdef USE_MODBUS
ModbusController modbusController(&dataStore, &swSource, &fanControl);
#endif

void play2BeepSoundWithOutDelay() {
  tone(ACTIVE_BUZZER_PIN, 4000, 1);
  tone(ACTIVE_BUZZER_PIN, 4000, 1);
}

void IRAM_ATTR BTN_UTIL_3_ISR() {}

bool exIsrCount = false;

void IRAM_ATTR EXATERNAL_ISR() { exIsrCount = true; }

void initBtnUtility() {
  pinMode(BTN_UTIL_PIN_3, INPUT_PULLUP);
  attachInterrupt(BTN_UTIL_PIN_3, BTN_UTIL_3_ISR, FALLING);
}

void loadArgbConfig() {
  ws2812fx.init();
  ws2812fx.setBrightness(dataStore.configData.argb.brightness);
  ws2812fx.setMode(dataStore.configData.argb.mode);
  ws2812fx.setSpeed(dataStore.configData.argb.speed);
  ws2812fx.setColor(dataStore.configData.argb.color);
  ws2812fx.start();
}

void mainLoopTask(void *parameter) {
  while (true) {
    oled.service(dataStore);
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void argbTask(void *parameter) {
  while (true) {
    ws2812fx.service();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void fanTasks(void *parameter) {
  for (;;) {
    fanControl.finalizePcnt();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

const char *ntpServer = "pool.ntp.org";
bool syncTimeFlag = false;
const long gmtOffset_sec = 7 * 3600;
const int daylightOffset_sec = 0;

#ifdef USE_RTC
void syncTime() {
  FANHUB_DEBUG_PRINTLN("Sync time");

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  syncTimeFlag = true;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    FANHUB_DEBUG_PRINTLN("Failed to obtain time");
    return;
  }
  FANHUB_DEBUG_PRINTLN(&timeinfo, "%A, %B %d %Y %H:%M:%S");

  if (timeinfo.tm_year > 100) {
    rtc.adjust(DateTime(timeinfo.tm_year, timeinfo.tm_mon, timeinfo.tm_mday,
                        timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
  }
  syncTimeFlag = true;
}
#endif

void onUpdateCallback(float progress) {
  oled.setOnUpdate(true);
  oled.displayUpdate(progress);
}

void setupService() {
#if defined(FANHUB_DEBUG) && !defined(USE_MODBUS)
  Serial.begin(SERIAL_BAUD_RATE);
#endif
  Wire.begin(SCL_PIN, SDA_PIN);
  dataStore.setResetPin(EEPROM_RESET_PIN);
  dataStore.begin();
  dataStore.saveDefaultConfigData();
  loadArgbConfig();
  swSource.begin();
  swSource.initSource(dataStore.configData.fanSource,
                      dataStore.configData.argb.source);
  fanControl.begin();
  fanControl.initAllDuty(dataStore.configData.fanDuty);
  initBtnUtility();
  voltageSensor.begin();
  molexPowerMeter.begin();
  thermister.begin(ADS1115_I2C_ADDRESS);
  boardTempSensor.begin();

#ifdef USE_MODBUS
  modbusController.begin();
#endif

  wifiConfig.useLedStatus(ARGB_DEBUG_PIN, DEBUG_LED_NUM, ARGB_BRIGHTNESS);
  wifiConfig.useArgbStrip(ws2812fx);
  wifiConfig.setOnUpdateCallback(onUpdateCallback);
  wifiConfig.begin();
#ifdef USE_RTC
  wifiConfig.setRtcUpdateCallback(syncTime);
  oled.setRtc(&rtc);
#endif
  oled.setStrip(&ws2812fx);
  oled.begin();
#ifdef USE_RTC
  if (!rtc.begin()) {
    FANHUB_DEBUG_PRINTLN("Couldn't find RTC");
  } else {
    FANHUB_DEBUG_PRINTLN("Found RTC");

    if (rtc.lostPower()) {
      FANHUB_DEBUG_PRINTLN("RTC lost power, let's set the time!");
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
#ifdef USE_RTC_DS3231
    rtc.disable32K();
    rtc.writeSqwPinMode(DS3231_SquareWave1Hz);
#endif
#ifdef USE_RTC_PCF8563
    rtc.writeSqwPinMode(PCF8563_SquareWave1Hz);
#endif
    pinMode(EXTERNAL_INTERUPT_PIN, INPUT_PULLUP);
    attachInterrupt(EXTERNAL_INTERUPT_PIN, EXATERNAL_ISR, FALLING);
  }
#endif

  if (!psramInit()) {
    FANHUB_DEBUG_PRINTLN("PSRAM initialization failed!");
  } else {
    FANHUB_DEBUG_PRINTLN("PSRAM initialized successfully");
  }

  xTaskCreatePinnedToCore(argbTask,   // Task function
                          "argbTask", // Name of the task
                          10000,      // Stack size (in bytes)
                          NULL,       // Task input parameter
                          1,          // Priority of the task
                          NULL,       // Task handle
                          1           // Core number (0 or 1)
  );

  xTaskCreatePinnedToCore(mainLoopTask,   // Task function
                          "mainLoopTask", // Name of the task
                          10000,          // Stack size (in bytes)
                          NULL,           // Task input parameter
                          2,              // Priority of the task
                          NULL,           // Task handle
                          1               // Core number (0 or 1)
  );

  // xTaskCreatePinnedToCore(
  //     fanTasks,   // Task function
  //     "fanTask", // Name of the task
  //     10000,     // Stack size (in bytes)
  //     NULL,      // Task input parameter
  //     2,         // Priority of the task
  //     NULL,      // Task handle
  //     1          // Core number (0 or 1)
  // );

  pinMode(20, OUTPUT);
}
unsigned long lastMillis = 0;
unsigned long lastIntervalMillis = 0;
void loopService() {
  unsigned long currentMillis = millis();
  wifiConfig.service();
  dataStore.service();
  // ws2812fx.service();
  wifiConfig.broadcastSensorData();

  if (lastIntervalMillis - currentMillis > 1000) {
    boardTempSensor.readSensors(dataStore.sensorData.boardTemp.temp,
                                dataStore.sensorData.boardTemp.humi,
                                dataStore.sensorData.boardTemp.cpuTemp);
    dataStore.setThermisterData(thermister);
    swSource.readState(dataStore.configData.fanSource,
                       dataStore.configData.argb.source);
    dataStore.setVoltageSensorData(voltageSensor);
    dataStore.setMolexPowerData(molexPowerMeter);
    fanControl.readFanData(&dataStore.sensorData.fanData);
    lastIntervalMillis = currentMillis;
    play2BeepSoundWithOutDelay();
  }

  if (exIsrCount) {
    fanControl.finalizePcnt();
    exIsrCount = false;
  }

#ifdef USE_RTC
  if (!syncTimeFlag && wifiConfig.ready()) {
    DateTime now = rtc.now();
    if (now.year() > 100) {
      syncTimeFlag = true;
      FANHUB_DEBUG_PRINTF("%d-%02d-%02d %02d:%02d:%02d\n", now.year(),
                          now.month(), now.day(), now.hour(), now.minute(),
                          now.second());
      FANHUB_DEBUG_PRINTLN("Skip time sync");
      return;
    }
  }
#endif

#ifdef USE_MODBUS
  modbusController.service();
#endif
}
