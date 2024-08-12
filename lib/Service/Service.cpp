#include <Service.h>

int ARGB_BRIGHTNESS = 60;
int ARGB_MODE = 1;
bool ARGB_FILLED = false;
WS2812FX ws2812fx = WS2812FX(NUM_LEDS, ARGB_PIN, NEO_GRB + NEO_KHZ800);

/// Define Sensors
VoltageSensor voltageSensor(INA219_5V_I2C_ADDRESS, INA219_12V_I2C_ADDRESS);
MolexPowerMeter molexPowerMeter(INA3221_MOLEX_I2C_ADDRESS);
BoardTempSensor boardTempSensor;
Oled oled;
Thermister thermister;
SwitchSource swSource(PCF8574_I2C_ADDRESS);
DataStore dataStore(EEPROM_I2C_ADDRESS, I2C_DEVICESIZE_24LC256, EEPROM_RESET_PIN);
FanControl fanControl;
WiFiConfig wifiConfig(&dataStore, &ws2812fx);

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
  thermister.begin();
  boardTempSensor.begin();
  wifiConfig.begin(ws2812fx, swSource, dataStore, fanControl);
  oled.begin();

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

  xTaskCreatePinnedToCore(
      fanTasks,   // Task function
      "fanTasks", // Name of the task
      10000,      // Stack size (in bytes)
      NULL,       // Task input parameter
      2,          // Priority of the task
      NULL,       // Task handle
      0           // Core number (0 or 1)
  );              // Enable the alarm                // Enable the alarm
}
unsigned long lastMillis = 0;
unsigned long lastIntervalMillis = 0;
void loopService()
{
  unsigned long currentMillis = millis();
  wifiConfig.service();
  dataStore.service();
  if (wifiConfig.ws.count() >= 1 && wifiConfig.ready() && currentMillis - lastMillis > 1000)
  {
    String out = dataStore.getSensorDataJson();
    wifiConfig.ws.textAll(out);
    lastMillis = currentMillis;
  }

  if (lastIntervalMillis - currentMillis > 1000)
  {
    boardTempSensor.readSensors(dataStore.sensorData.boardTemp.temp, dataStore.sensorData.boardTemp.humi, dataStore.sensorData.boardTemp.cpuTemp);
    dataStore.setThermisterData(thermister);
    swSource.readState(dataStore.configData.fanSource, dataStore.configData.argb.source);
    dataStore.setVoltageSensorData(voltageSensor);
    fanControl.readFanData(&dataStore.sensorData.fanData);
    lastIntervalMillis = currentMillis;
  }
}
