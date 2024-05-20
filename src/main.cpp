#include <Arduino.h>
#include <Wire.h>
#include <SensirionI2cSht4x.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_INA219.h>
#include "PCF8574.h"

#define PIN_TAC_1 41
#define PIN_TAC_2 40
#define PIN_TAC_3 39
#define PIN_TAC_4 38
#define PIN_TAC_5 42

#define PIN_PWM_1 6
#define PIN_PWM_2 5
#define PIN_PWM_3 4
#define PIN_PWM_4 20
#define PIN_PWM_5 19

#define PWM_CH_1 0
#define PWM_CH_2 1
#define PWM_CH_3 2
#define PWM_CH_4 3
#define PWM_CH_5 4

#define PWM_FREQ 25000
#define PWM_RES 8
#define PWM_DEFAULT_DUTY 127

#define BTN_UTIL_PIN_1 15 // PWM +
#define BTN_UTIL_PIN_2 16 // PWM -
#define BTN_UTIL_PIN_3 17 // ARGB RND

int PWM_FREQ_1 = 0;
int PWM_FREQ_2 = 0;
int PWM_FREQ_3 = 0;
int PWM_FREQ_4 = 0;
int PWM_FREQ_5 = 0;

int PWM_DUTY_1 = PWM_DEFAULT_DUTY;
int PWM_DUTY_2 = PWM_DEFAULT_DUTY;
int PWM_DUTY_3 = PWM_DEFAULT_DUTY;
int PWM_DUTY_4 = PWM_DEFAULT_DUTY;
int PWM_DUTY_5 = PWM_DEFAULT_DUTY;

#define ARGB_PIN 8
#define COLOR_ORDER GRB
#define CHIPSET WS2811
#define NUM_LEDS 40
#define BRIGHTNESS 200
#define FRAMES_PER_SECOND 60

int ARGB_BRIGHTNESS = 60;
int ARGB_MODE = 1;
bool ARGB_FILLED = false;
Adafruit_NeoPixel pixels(NUM_LEDS, ARGB_PIN, NEO_GRB + NEO_KHZ800);
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

#define INA219_1_I2C_ADDRESS 0x41
#define INA219_2_I2C_ADDRESS 0x45

Adafruit_INA219 ina219_1(INA219_1_I2C_ADDRESS);
Adafruit_INA219 ina219_2(INA219_2_I2C_ADDRESS);

void initIna219()
{
  if (!ina219_1.begin(&Wire))
  {
    Serial.println("Failed to find INA219 1 chip");
  }
  if (!ina219_2.begin(&Wire))
  {
    Serial.println("Failed to find INA219 2 chip");
  }
}

void readInaData(Adafruit_INA219 &ina219)
{
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);

  Serial.print("\n========================================\n");
  Serial.print("Bus Voltage:   ");
  Serial.print(busvoltage);
  Serial.println(" V");
  Serial.print("Shunt Voltage: ");
  Serial.print(shuntvoltage);
  Serial.println(" mV");
  Serial.print("Load Voltage:  ");
  Serial.print(loadvoltage);
  Serial.println(" V");
  Serial.print("Current:       ");
  Serial.print(current_mA);
  Serial.println(" mA");
  Serial.print("Power:         ");
  Serial.print(power_mW);
  Serial.println(" mW");
  Serial.print("========================================\n");
}

void initArgb()
{
  pixels.clear();
  pixels.show();
  pixels.setBrightness(ARGB_BRIGHTNESS);
  // set green color and show on pixel 1
  pixels.setPixelColor(0, 0xFF00FF00);
  pixels.show();
}

#define ADS1115_I2C_ADDRESS 0x48
Adafruit_ADS1115 ads;

bool adsInit = false;

void initAds1115()
{
  if (!ads.begin(ADS1115_I2C_ADDRESS, &Wire))
  {
    Serial.println("ADS1115 not connected!");
    return;
  }
  Serial.println("ADS1115 connected!");
  adsInit = true;
  // ads.setVoltageRange_mV(ADS1115_RANGE_6144);
  // ads.setCompareChannels(ADS1115_COMP_0_GND);
}

const float R0 = 10000.0;          // Resistor value in ohms
const float beta = 3950.0;         // Beta value of the NTC thermistor
const float T0 = 298.15;           // Temperature in Kelvin at R0 (25°C)
const float ADC_REF_VOLTAGE = 5.0; // Reference voltage for ADC
const float ADJUST_ADC_VALUE = 0;  // Reference voltage for ADC
const float TEMP_OFFSET = -11;     // Offset for temperature readings

float calculateTemperature(float resistance)
{
  // Using the Beta parameter equation
  // return 1.0 / ((log(resistance / R0) / beta) + (1.0 / T0)) - 273.15;
  // return (1.0 / ((log(resistance / R0) / beta) + (1.0 / T0))) - 273.15;
  return (1.0 / ((log(R0 / resistance) / beta) + (1.0 / T0))) - 273.15;
}

void readTemp(int channel)
{
  int16_t adc_value = ads.readADC_SingleEnded(channel);

  // Adjust the ADC value if necessary
  adc_value += ADJUST_ADC_VALUE;

  // Calculate the voltage from the ADC value
  float voltage = adc_value * (ADC_REF_VOLTAGE / 32767.0);

  // Calculate the resistance of the thermistor
  float resistance = R0 * (ADC_REF_VOLTAGE / voltage - 1.0);

  // Calculate temperature using the Beta parameter equation
  float tempC = calculateTemperature(resistance);
  tempC += TEMP_OFFSET;
  // Print the results
  Serial.print("\n========================================\n");
  Serial.print("ADC Value: ");
  Serial.println(adc_value);
  Serial.print("Voltage: ");
  Serial.println(voltage);
  Serial.print("Resistance: ");
  Serial.println(resistance);
  Serial.print("Temperature: ");
  Serial.println(tempC);
  Serial.print("========================================\n");
}

#define NO_ERROR 0

SensirionI2cSht4x sht;
void initSht40()
{
  char errorMessage[64];
  int16_t error;
  sht.begin(Wire, SHT40_I2C_ADDR_44);
  sht.softReset();
  delay(10);
  uint32_t serialNumber = 0;
  error = sht.serialNumber(serialNumber);
  if (error != NO_ERROR)
  {
    Serial.print("Error trying to execute serialNumber(): ");
    errorToString(error, errorMessage, sizeof errorMessage);
    Serial.println(errorMessage);
    return;
  }
  Serial.print("serialNumber: ");
  Serial.print(serialNumber);
  Serial.println();
}

void readSht40()
{
  char errorMessage[64];
  int16_t error;
  float aTemperature = 0.0;
  float aHumidity = 0.0;
  delay(20);
  error = sht.measureLowestPrecision(aTemperature, aHumidity);
  if (error != NO_ERROR)
  {
    Serial.print("Error trying to execute measureLowestPrecision(): ");
    errorToString(error, errorMessage, sizeof errorMessage);
    Serial.println(errorMessage);
    return;
  }
  Serial.print("========================================\n");
  Serial.print("Temp: ");
  Serial.print(aTemperature);
  Serial.print("\t");
  Serial.print("Hum: ");
  Serial.print(aHumidity);
  Serial.print("\n========================================\n");
}

#define PCF8574_I2C_ADDRESS 0x39
PCF8574 pcf(PCF8574_I2C_ADDRESS);

void initPcf8574()
{
  for (int i = 0; i < 8; i++)
  {
    pcf.pinMode(i, OUTPUT);
  }
  Serial.print("Init pcf8574...");
  if (pcf.begin())
  {
    Serial.println("OK");
    for (int i = 0; i < 8; i++)
    {
      pcf.digitalWrite(i, LOW);
    }
  }
  else
  {
    Serial.println("KO");
  }
  /// Test Write switch hight
  pcf.digitalWrite(3, HIGH);
}

void IRAM_ATTR TAC_1_ISR()
{
  PWM_FREQ_1++;
}

void IRAM_ATTR TAC_2_ISR()
{
  PWM_FREQ_2++;
}

void IRAM_ATTR TAC_3_ISR()
{
  PWM_FREQ_3++;
}

void IRAM_ATTR TAC_4_ISR()
{
  PWM_FREQ_4++;
}

void IRAM_ATTR TAC_5_ISR()
{
  PWM_FREQ_5++;
}

void IRAM_ATTR BTN_UTIL_1_ISR()
{
  int NEW_DUTY = PWM_DUTY_1 + 10;
  if (NEW_DUTY > 255)
  {
    NEW_DUTY = 255;
  }
  ledcWrite(PWM_CH_1, NEW_DUTY);
  ledcWrite(PWM_CH_2, NEW_DUTY);
  ledcWrite(PWM_CH_3, NEW_DUTY);
  ledcWrite(PWM_CH_4, NEW_DUTY);
  ledcWrite(PWM_CH_5, NEW_DUTY);

  PWM_DUTY_1 = NEW_DUTY;
  PWM_DUTY_2 = NEW_DUTY;
  PWM_DUTY_3 = NEW_DUTY;
  PWM_DUTY_4 = NEW_DUTY;
  PWM_DUTY_5 = NEW_DUTY;
}

void IRAM_ATTR BTN_UTIL_2_ISR()
{
  int NEW_DUTY = PWM_DUTY_1 - 10;
  if (NEW_DUTY < 0)
  {
    NEW_DUTY = 0;
  }
  ledcWrite(PWM_CH_1, NEW_DUTY);
  ledcWrite(PWM_CH_2, NEW_DUTY);
  ledcWrite(PWM_CH_3, NEW_DUTY);
  ledcWrite(PWM_CH_4, NEW_DUTY);
  ledcWrite(PWM_CH_5, NEW_DUTY);
  PWM_DUTY_1 = NEW_DUTY;
  PWM_DUTY_2 = NEW_DUTY;
  PWM_DUTY_3 = NEW_DUTY;
  PWM_DUTY_4 = NEW_DUTY;
  PWM_DUTY_5 = NEW_DUTY;
}

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

void initTachometer()
{
  pinMode(PIN_TAC_1, INPUT_PULLUP);
  pinMode(PIN_TAC_2, INPUT_PULLUP);
  pinMode(PIN_TAC_3, INPUT_PULLUP);
  pinMode(PIN_TAC_4, INPUT_PULLUP);
  pinMode(PIN_TAC_5, INPUT_PULLUP);

  attachInterrupt(PIN_TAC_1, TAC_1_ISR, FALLING);
  attachInterrupt(PIN_TAC_2, TAC_2_ISR, FALLING);
  attachInterrupt(PIN_TAC_3, TAC_3_ISR, FALLING);
  attachInterrupt(PIN_TAC_4, TAC_4_ISR, FALLING);
  attachInterrupt(PIN_TAC_5, TAC_5_ISR, FALLING);
}

void initBtnUtility()
{
  pinMode(BTN_UTIL_PIN_1, INPUT_PULLUP);
  pinMode(BTN_UTIL_PIN_2, INPUT_PULLUP);
  pinMode(BTN_UTIL_PIN_3, INPUT_PULLUP);
  attachInterrupt(BTN_UTIL_PIN_1, BTN_UTIL_1_ISR, FALLING);
  attachInterrupt(BTN_UTIL_PIN_2, BTN_UTIL_2_ISR, FALLING);
  attachInterrupt(BTN_UTIL_PIN_3, BTN_UTIL_3_ISR, FALLING);
}

void initPwmGenerator()
{
  ledcSetup(PWM_CH_1, PWM_FREQ, PWM_RES);
  ledcSetup(PWM_CH_2, PWM_FREQ, PWM_RES);
  ledcSetup(PWM_CH_3, PWM_FREQ, PWM_RES);
  ledcSetup(PWM_CH_4, PWM_FREQ, PWM_RES);
  ledcSetup(PWM_CH_5, PWM_FREQ, PWM_RES);

  ledcAttachPin(PIN_PWM_1, PWM_CH_1);
  ledcAttachPin(PIN_PWM_2, PWM_CH_2);
  ledcAttachPin(PIN_PWM_3, PWM_CH_3);
  ledcAttachPin(PIN_PWM_4, PWM_CH_4);
  ledcAttachPin(PIN_PWM_5, PWM_CH_4);

  ledcWrite(PWM_CH_1, PWM_DUTY_1);
  ledcWrite(PWM_CH_2, PWM_DUTY_2);
  ledcWrite(PWM_CH_3, PWM_DUTY_3);
  ledcWrite(PWM_CH_4, PWM_DUTY_4);
  ledcWrite(PWM_CH_5, PWM_DUTY_5);
}

void setup()
{
  Serial.begin(115200);
  Wire.begin(12, 11);
  initTachometer();
  initPwmGenerator();
  initBtnUtility();
  initArgb();
  initPcf8574();
  initIna219();
  initAds1115();
  initSht40();
}

void loop()
{
  // Serial.printf("Freq: %d \n", PWM_FREQ_1);
  // Serial.printf("Duty: %d \n", PWM_DUTY_1);
  if (testColorsIndexChanged)
  {
    uint32_t color = argbTestColors[testColorsIndex];
    pixels.fill(color);
    pixels.show();
    testColorsIndexChanged = false;
  }

  // for (int i = 0; i < 8; i++)
  // {
  //   pcf.digitalWrite(i, HIGH);
  // }
  // delay(1000);
  // for (int i = 0; i < 8; i++)
  // {
  //   pcf.digitalWrite(i, LOW);
  // }
  // delay(1000);
  if (adsInit)
  {
    readTemp(0);
    readTemp(1);
    // readTemp(1);
  }

  readSht40();

  // readInaData(ina219_1);
  // readInaData(ina219_2);

  delay(2000);
}
