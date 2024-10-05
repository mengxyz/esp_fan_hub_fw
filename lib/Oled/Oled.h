#if !defined(OLED_H)
#define OLED_H

#include <Arduino.h>
#include <BaseModule.h>
#include <Adafruit_SSD1306.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <DataStore.h>
#include <RTClib.h>
#include <WS2812FX.h>

#define OLED_I2C_ADDRESS 0x3C

const uint16_t OLED_UPDATE_INTERVAL = 1000;
const uint16_t OLED_PAGE_CHANGE_INTERVAL = 34464;
const uint8_t OLED_PAGE_TIME_OUT = 30;
const uint8_t OLED_PAGE_SIZE = 6;
const uint8_t OLED_STARTUP_PAGE_TIME_OUT = 10;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

class Oled {
public:
    Oled();
    void begin();
    void service(DataStore &dataStore);
	void setRtc(RTC_DS3231 *rtc) {
		this->rtc = rtc;
	}

    void setStrip(WS2812FX *strip) {
        this->strip = strip;
    }

private:
    Adafruit_SSD1306 display;
    WS2812FX *strip;
    unsigned long latestUpdateMillis = 0;
    int currentPage = 0;
    int tickPageChange = 0;
	RTC_DS3231 *rtc;
    U8G2_FOR_ADAFRUIT_GFX u8g2Writer;
    void displayData(DataStore &dataStore);
    void displaySplash();
    void displayFanData(SensorData &sensorData);
    void displayTempHumidity(SensorData &sensorData);
    void displayOtherSensors(SensorData &sensorData);
    void displayPowerConsumption(SensorData &sensorData);
    void displayLightInfo(DataStore &dataStore);
};


#endif
