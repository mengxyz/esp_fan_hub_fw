#include "Oled.h"

Oled::Oled() : u8g2(U8G2_R0, U8X8_PIN_NONE)
{
}

void Oled::begin()
{
    u8g2.setI2CAddress(OLED_I2C_ADDRESS);
    if (u8g2.begin())
    {
        Serial.println("OLED ready");
        isReady = true;
        return;
    }
    Serial.println("OLED not ready");
    return;
}

void Oled::service(SensorData &sensorData)
{
    unsigned long currentMillis = millis();

    if (currentMillis - latestUpdateMillis >= OLED_UPDATE_INTERVAL)
    {
        // Serial.printf("TICK %d \n", tickPageChange);
        u8g2.firstPage();
        do
        {
            displayData(sensorData);
        } while (u8g2.nextPage());
        latestUpdateMillis = currentMillis;
        if (tickPageChange >= OLED_PAGE_PAGETICK)
        {
            currentPage = (currentPage + 1) % 4;
            tickPageChange = 0;
        }
        else
        {
            if (tickPageChange >= OLED_STARTUP_PAGE_TICK && currentPage == 0)
            {
                currentPage = 1;
                tickPageChange = 0;
            }
            else
            {
                tickPageChange++;
            }
        }
    }
}

void Oled::displayData(SensorData &sensorData)
{
    static int lastPage = -1;
    if (lastPage != currentPage)
    {
        u8g2.clearBuffer();
        lastPage = currentPage;
    }

    u8g2.setFont(u8g2_font_spleen5x8_mr);

    switch (currentPage)
    {
    case 0:
        displaySplash();
        break;
    case 1:
        displayFanData(sensorData);
        break;
    case 2:
        displayTempHumidity(sensorData);
        break;
    case 3:
        displayOtherSensors(sensorData);
        break;
    }

    // Draw progress bar at the bottom
    int progressWidth = (tickPageChange * 128) / OLED_PAGE_PAGETICK; // 128 is the width of the OLED display
    u8g2.drawBox(0, 60, progressWidth, 1);                           // 60 is the y-coordinate for the bottom of the screen, 4 is the height of the bar

    u8g2.sendBuffer();
}

void Oled::displaySplash()
{
    u8g2.setCursor(0, 8);
    u8g2.print("Welcome to");
    u8g2.setCursor(0, 16);
    u8g2.print("Esp Fan Hub");
    u8g2.setCursor(0, 24);
    u8g2.print("By: mengxyz");
}

void Oled::displayFanData(SensorData &sensorData)
{
    for (int i = 0; i < 5; i++)
    {
        u8g2.setCursor(4, 8 + 4 + i * 8);
        u8g2.print("Fan ");
        u8g2.print(i + 1);
        u8g2.print(": RPM ");
        u8g2.print(sensorData.fanData.rpm[i]);
        u8g2.print(", Duty ");
        u8g2.print(int(ceil(sensorData.fanData.duty[i] * 100 / 255)));
        u8g2.print("%");
    }
}

void Oled::displayTempHumidity(SensorData &sensorData)
{
    u8g2.setCursor(4, 8 + 4);
    u8g2.print("Temp: ");
    u8g2.print(sensorData.boardTemp.temp);
    u8g2.print("C");

    u8g2.setCursor(4, 16 + 4);
    u8g2.print("Humidity: ");
    u8g2.print(sensorData.boardTemp.humi);
    u8g2.print("%");
}

void Oled::displayOtherSensors(SensorData &sensorData)
{
    u8g2.setCursor(4, 8 + 4);
    u8g2.print("NTC1: ");
    u8g2.print(sensorData.thermistor.ch0Temp);
    u8g2.print("C");

    u8g2.setCursor(4, 16 + 4);
    u8g2.print("NTC2: ");
    u8g2.print(sensorData.thermistor.ch1Temp);
    u8g2.print("C");

    u8g2.setCursor(4, 24 + 4);
    u8g2.print("Controller: ");
    u8g2.print(0.0);
    u8g2.print("C");

    u8g2.setCursor(4, 32 + 4);
    u8g2.print("Water Flow: ");
    u8g2.print(0.0);
    u8g2.print("L/min");
}