#if !defined(SERVICE_H)
#define SERVICE_H

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
#include <ConfigData.h>

#define BTN_UTIL_PIN_3 17 // ARGB RND

#define ARGB_PIN 8
#define ARGB_DEBUG_PIN 3
#define COLOR_ORDER GRB
#define CHIPSET WS2811
#define NUM_LEDS 40
#define BRIGHTNESS 200
#define FRAMES_PER_SECOND 60

#define PCF8574_I2C_ADDRESS 0x21
#define INA219_5V_I2C_ADDRESS 0x40
#define INA219_12V_I2C_ADDRESS 0x41
#define INA3221_MOLEX_I2C_ADDRESS 0x43
#define SHT30_I2C_ADDRESS 0x44
#define ADS1115_I2C_ADDRESS 0x48
#define EEPROM_I2C_ADDRESS 0x50
#define EEPROM_I2C_ADDRESS 0x50
#define DS3231_I2C_ADDRESS 0x68
#define SERIAL_BAUD_RATE 115200
// #define SCL_PIN 11
// #define SDA_PIN 12
#define SCL_PIN 14
#define SDA_PIN 13
#define EEPROM_RESET_PIN 12


void setupService();
void loopService();


#endif // SERVICE_H
