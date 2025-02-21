#if !defined(SERVICE_H)
#define SERVICE_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
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
#include <RTClib.h>
// #include <ActionButton.h>
#include <time.h>
#include <UserConfig.h>
#include <ModbusController.h>


void setupService();
void loopService();


#endif // SERVICE_H
