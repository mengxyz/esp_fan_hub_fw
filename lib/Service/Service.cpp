#include <Service.h>

int ARGB_BRIGHTNESS = 60;
int ARGB_MODE = 1;
bool ARGB_FILLED = false;
WS2812FX ws2812fx = WS2812FX(NUM_LEDS, ARGB_PIN, NEO_GRB + NEO_KHZ800);
// WS2812FX ws2812fx = WS2812FX(NUM_LEDS, ARGB_DEBUG_PIN, NEO_GRB + NEO_KHZ800);

/// Define Sensors
VoltageSensor voltageSensor(INA219_5V_I2C_ADDRESS, INA219_12V_I2C_ADDRESS);
MolexPowerMeter molexPowerMeter(INA3221_MOLEX_I2C_ADDRESS);
BoardTempSensor boardTempSensor(SHTSensor::SHT3X);
Oled oled;
Thermister thermister;
SwitchSource swSource(PCF8574_I2C_ADDRESS);
DataStore dataStore(EEPROM_I2C_ADDRESS, I2C_DEVICESIZE_24LC256);
FanControl fanControl;
WiFiConfig wifiConfig(&dataStore, &swSource, &fanControl);
RTC_DS3231 rtc;
// ActionButton actionBtn(EEPROM_RESET_PIN, 2, 20);

void IRAM_ATTR BTN_UTIL_3_ISR()
{
}

bool exIsrCount = false;

void IRAM_ATTR EXATERNAL_ISR()
{
  exIsrCount = true;
}

void initBtnUtility()
{
  pinMode(BTN_UTIL_PIN_3, INPUT_PULLUP);
  attachInterrupt(BTN_UTIL_PIN_3, BTN_UTIL_3_ISR, FALLING);
}

void loadArgbConfig()
{
  ws2812fx.init();
  ws2812fx.setBrightness(dataStore.configData.argb.brightness);
  ws2812fx.setMode(dataStore.configData.argb.mode);
  ws2812fx.setSpeed(dataStore.configData.argb.speed);
  ws2812fx.setColor(dataStore.configData.argb.color);
  ws2812fx.start();
}

void mainLoopTask(void *parameter)
{
  while (true)
  {
    oled.service(dataStore.sensorData);
    vTaskDelay(10 / portTICK_PERIOD_MS);
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

const char *ntpServer = "pool.ntp.org";
bool syncTimeFlag = false;
const long gmtOffset_sec = 7 * 3600;
const int daylightOffset_sec = 0;
void syncTime()
{
  Serial.println("Sync time");

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  syncTimeFlag = true;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

  if (timeinfo.tm_year > 100)
  {
    rtc.adjust(DateTime(timeinfo.tm_year, timeinfo.tm_mon, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
  }
  syncTimeFlag = true;
}

void setupService()
{
  Serial.begin(SERIAL_BAUD_RATE);
  Wire.begin(SCL_PIN, SDA_PIN);
  dataStore.begin();
  loadArgbConfig();
  swSource.begin();
  swSource.initSource(dataStore.configData.fanSource, dataStore.configData.argb.source);
  fanControl.begin();
  fanControl.initAllDuty(dataStore.configData.fanDuty);
  initBtnUtility();
  voltageSensor.begin();
  thermister.begin(ADS1115_I2C_ADDRESS);
  boardTempSensor.begin();
  wifiConfig.useLedStatus(ARGB_DEBUG_PIN, DEBUG_LED_NUM, ARGB_BRIGHTNESS);
  wifiConfig.useArgbStrip(ws2812fx);
  wifiConfig.begin();
  wifiConfig.setRtcUpdateCallback(syncTime);
  // oled.setRtc(&rtc);
  oled.begin();

  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
  }
  else
  {
    Serial.println("Found RTC");

    if (rtc.lostPower())
    {
      Serial.println("RTC lost power, let's set the time!");
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    rtc.disable32K();
    rtc.writeSqwPinMode(DS3231_SquareWave1Hz);
    pinMode(EXTERNAL_INTERUPT_PIN, INPUT_PULLUP);
    attachInterrupt(EXTERNAL_INTERUPT_PIN, EXATERNAL_ISR, FALLING);
  }

  if (!psramInit())
  {
    Serial.println("PSRAM initialization failed!");
  }
  else
  {
    Serial.println("PSRAM initialized successfully");
  }

  // actionBtn.addStep(1, (void (*) {

  // }));

  // actionBtn.addStep(5, (void (*) {
  //   dataStore.saveDefaultConfigData();
  //   ESP.restart();
  // }));

  // Create the main loop task and pin it to core 1
  xTaskCreatePinnedToCore(
      mainLoopTask,   // Task function
      "mainLoopTask", // Name of the task
      10000,          // Stack size (in bytes)
      NULL,           // Task input parameter
      1,              // Priority of the task
      NULL,           // Task handle
      0               // Core number (0 or 1)
  );

  // xTaskCreatePinnedToCore(
  //     fanTasks,   // Task function
  //     "fanTasks", // Name of the task
  //     10000,      // Stack size (in bytes)
  //     NULL,       // Task input parameter
  //     2,          // Priority of the task
  //     NULL,       // Task handle
  //     0           // Core number (0 or 1)
  // );              // Enable the alarm                // Enable the alarm
}
unsigned long lastMillis = 0;
unsigned long lastIntervalMillis = 0;
void loopService()
{
  unsigned long currentMillis = millis();
  wifiConfig.service();
  dataStore.service();
  ws2812fx.service();
  wifiConfig.broadcastSensorData();

  if (lastIntervalMillis - currentMillis > 1000)
  {
    boardTempSensor.readSensors(dataStore.sensorData.boardTemp.temp, dataStore.sensorData.boardTemp.humi, dataStore.sensorData.boardTemp.cpuTemp);
    dataStore.setThermisterData(thermister);
    swSource.readState(dataStore.configData.fanSource, dataStore.configData.argb.source);
    dataStore.setVoltageSensorData(voltageSensor);
    fanControl.readFanData(&dataStore.sensorData.fanData);
    lastIntervalMillis = currentMillis;
  }

  if (exIsrCount)
  {
    fanControl.finalizePcnt();
    exIsrCount = false;
  }

  if (!syncTimeFlag && wifiConfig.ready())
  {
    DateTime now = rtc.now();
    if (now.year() > 100)
    {
      syncTimeFlag = true;
      Serial.printf("%d-%02d-%02d %02d:%02d:%02d\n", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
      Serial.println("Skip time sync");
      return;
    }
  }
}
