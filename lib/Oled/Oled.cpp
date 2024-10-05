#include <Oled.h>
#include <Fonts/Picopixel.h>
#include <WiFi.h>
#include <WS2812FX.h>

const uint8_t u8g2_font_spleen5x8_mr[1010] U8G2_FONT_SECTION("u8g2_font_spleen5x8_mr") = 
  "`\2\3\2\3\4\1\1\4\5\10\0\377\6\377\7\377\1\77\2\217\3\325 \6\305\372\274\2!\10\305"
  "Zaw(\7\42\12\305:\245$JrV\0#\15\305\332I\62(\245$\31\224\62\0$\13\305Z"
  "\331R\23\65e\214\0%\15\305zI\224\24\263\60)%!\0&\16\305ZY\22%\221\224$R\244"
  "\244\0'\7\305Za\235\31(\10\305z\215\255\25\0)\10\305:i\261\255\6*\13\305\372X\24I"
  "C$\225\1+\12\305\372h\30\15R\230\3,\10\305\372\314a\226\1-\10\305\372\344!'\1.\7"
  "\305\372\34s\0/\13\305za\26fa\26\206\0\60\12\305\332R%\261\224\42\35\61\10\305\372\231\330"
  "\66\3\62\12\305\332R\61\222\302!\6\63\12\305\332R-M\242H\7\64\14\305\272a\22%\321\220\205"
  "\71\0\65\12\305\272C\22\256a\262\3\66\12\305\332R\70U\242H\7\67\13\305\272C\22\205Y\61G"
  "\0\70\12\305\332RI\252D\221\16\71\12\305\332R%\212\306H\7:\10\305\372\264\34\317\1;\11\305"
  "\372\264\34\12\263\14<\11\305\372HVL\313\0=\11\305\372\224!\36r\20>\11\305\332i\61\253#"
  "\0\77\12\305:R\61\253C\71\2@\13\305\332R%Q\22%\235\1A\14\305\332R%J\206$J"
  "\242\30B\12\305\272Se\252D\311\16C\10\305\332K\330:\3D\14\305\272S%J\242$Jv\0"
  "E\11\305\332K\70\205\351\14F\12\305\332K\30Na\16\1G\14\305\332K\230(Q\22E\63\0H"
  "\16\305\272Q\22%C\22%Q\22\305\0I\10\305\332[\330\66\3J\11\305\332[\330\244#\0K\14"
  "\305\272Q\22%S%J\242\30L\7\305\272a\327\31M\16\305\272Q\62$C\22%Q\22\305\0N"
  "\15\305\272Q\242$JEI\224(\6O\14\305\332R%J\242$\212t\0P\13\305\272S%J\246"
  "\60\207\0Q\14\305\332R%J\242$\212D\5R\13\305\272S%J\246J\24\3S\11\305\332K\252"
  "\206\311\16T\10\305\272\203\24v\7U\15\305\272Q\22%Q\22%Q\64\3V\14\305\272Q\22%Q"
  "\22E\232\16W\16\305\272Q\22%Q\62$C\22\305\0X\14\305\272Q\22E\232T\211b\0Y\14"
  "\305\272Q\22%Q\64&;\0Z\12\305\272C\230\65\16\61\0[\10\305:S\330\343\2\134\13\305\32"
  "a\32\246a\32&\0]\10\305:c\237\26\0^\11\305\372YR\313\311\0_\7\305\372\334\207\4`"
  "\7\305:i\316\21a\12\305\372\240\32-Q\64\3b\14\305\32a\70U\242$Jv\0c\11\305\372"
  "\340\22Vg\0d\14\305za\264DI\224D\321\14e\13\305\372\340\22%C\222\316\0f\12\305Z"
  "R\230ma\35\1g\14\305\372\340\22%Q\244&\23\0h\14\305\32a\70U\242$J\242\30i\11"
  "\305\372\71\42\26e\0j\11\305\372\71\24\66i\0k\13\305\32a))iIT\6l\10\305:a"
  "\257\62\0m\15\305\372X\224\14\311\220DI\24\3n\14\305\372\330T\211\222(\211b\0o\13\305\372"
  "\240T\211\222(\322\1p\13\305\372\330T\211\222)\14\1q\13\305\372\340\22%Q\64V\0r\12\305"
  "\372\340\22%a\35\2s\11\305\372\340\222\252\311\16t\11\305:a\266\205U\31u\14\305\372X\224D"
  "I\224D\321\14v\14\305\372X\224DI\24i:\0w\15\305\372X\224D\311\220\14I\24\3x\13"
  "\305\372X\24iR%\212\1y\14\305\372X\224DI\24\215\311\4z\12\305\372\330\20f\265!\6{"
  "\12\305ZR\230\31\253\12\0|\7\305Za\77\1}\13\305\32j\30jZ\30i\0~\11\305\372\244"
  "H\321I\0\177\6\305\372\274\2\0\0\0\4\377\377\0";

Oled::Oled() : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET) {}

void Oled::begin()
{
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS))
    {
        Serial.println("SSD1306 allocation failed");
        return;
    }
    Serial.println("OLED ready");
    display.clearDisplay();
    display.display();
    u8g2Writer.begin(display);
}

void Oled::service(DataStore &dataStore)
{
    unsigned long currentMillis = millis();

    if (currentMillis - latestUpdateMillis >= OLED_UPDATE_INTERVAL)
    {
        display.clearDisplay();
        displayData(dataStore);
        display.display();
        latestUpdateMillis = currentMillis;

        if (tickPageChange >= OLED_PAGE_TIME_OUT)
        {
            currentPage = (currentPage + 1) % OLED_PAGE_SIZE;
            tickPageChange = 0;
        }
        else
        {
            if (tickPageChange >= OLED_STARTUP_PAGE_TIME_OUT && currentPage == 0)
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

void Oled::displayData(DataStore &dataStore)
{
    u8g2Writer.setFont(u8g2_font_spleen5x8_mr);

    switch (currentPage)
    {
    case 0:
        displaySplash();
        break;
    case 1:
        displayLightInfo(dataStore);
        break;
    case 2:
        displayPowerConsumption(dataStore.sensorData);
        break;
    case 3:
        displayFanData(dataStore.sensorData);
        break;
    case 4:
        displayTempHumidity(dataStore.sensorData);
        break;
    case 5:
        displayOtherSensors(dataStore.sensorData);
        break;
    }

    if (rtc != NULL)
    {
        DateTime now = rtc->now();
        char timeBuffer[20];
        snprintf(timeBuffer, sizeof(timeBuffer), "%02d/%02d/%02d %02d:%02d:%02d",
                 now.day(), now.month(), now.year() % 100, now.hour(), now.minute(), now.second());

        u8g2Writer.setCursor(4, 56);
        u8g2Writer.print(timeBuffer);
    }

    int progressWidth = (tickPageChange * 128) / OLED_PAGE_TIME_OUT;
    display.fillRect(0, 60, progressWidth, 4, SSD1306_WHITE);
}

void Oled::displaySplash()
{
    u8g2Writer.setCursor(0, 8);
    u8g2Writer.print("Welcome to");
    u8g2Writer.setCursor(0, 16);
    u8g2Writer.print("Esp Fan Hub");
    u8g2Writer.setCursor(0, 24);
    u8g2Writer.print("By: mengxyz");
    u8g2Writer.setCursor(0, 32);
    u8g2Writer.print("Wifi Status: ");
    if (WiFi.status() == WL_CONNECTED)
    {
        u8g2Writer.print("Connected");
        u8g2Writer.setCursor(0, 40);
        u8g2Writer.print("IP: ");
        u8g2Writer.print(WiFi.localIP());

        u8g2Writer.setCursor(0, 48);
        u8g2Writer.print("RSSI: ");
        u8g2Writer.print(WiFi.RSSI());
    }
    else
    {
        u8g2Writer.print("Disconnected");
    }
}

void Oled::displayFanData(SensorData &sensorData)
{
    for (int i = 0; i < 5; i++)
    {
        u8g2Writer.setCursor(4, 8 + i * 8);
        u8g2Writer.print("Fan ");
        u8g2Writer.print(i + 1);
        u8g2Writer.print(": RPM ");
        u8g2Writer.print(sensorData.fanData.rpm[i]);
        u8g2Writer.print(", Duty ");
        u8g2Writer.print(int(ceil(sensorData.fanData.duty[i] * 100 / 255)));
        u8g2Writer.print("%");
    }
}

void Oled::displayTempHumidity(SensorData &sensorData)
{
    u8g2Writer.setCursor(4, 8);
    u8g2Writer.print("Temp: ");
    u8g2Writer.print(sensorData.boardTemp.temp);
    u8g2Writer.print("C");

    u8g2Writer.setCursor(4, 16);
    u8g2Writer.print("Humidity: ");
    u8g2Writer.print(sensorData.boardTemp.humi);
    u8g2Writer.print("%");
}

void Oled::displayOtherSensors(SensorData &sensorData)
{
    u8g2Writer.setCursor(4, 8);
    u8g2Writer.print("NTC1: ");
    u8g2Writer.print(sensorData.thermistor.ch0Temp);
    u8g2Writer.print("C");

    u8g2Writer.setCursor(4, 16);
    u8g2Writer.print("NTC2: ");
    u8g2Writer.print(sensorData.thermistor.ch1Temp);
    u8g2Writer.print("C");

    u8g2Writer.setCursor(4, 24);
    u8g2Writer.print("Controller: ");
    u8g2Writer.print(0.0);
    u8g2Writer.print("C");

    u8g2Writer.setCursor(4, 32);
    u8g2Writer.print("Water Flow: ");
    u8g2Writer.print(0.0);
    u8g2Writer.print("L/min");
}

void Oled::displayPowerConsumption(SensorData &sensorData)
{
    u8g2Writer.setCursor(4, 8);
    u8g2Writer.print("12V: ");
    u8g2Writer.print(sensorData.voltage.twelveVolt.voltage);
    u8g2Writer.setCursor(4, 16);
    // display as W from mW
    u8g2Writer.print("POWER: ");
    u8g2Writer.print(sensorData.voltage.twelveVolt.power / 1000);
    u8g2Writer.print(" W | ");
    u8g2Writer.print(sensorData.voltage.twelveVolt.current);
    u8g2Writer.print(" mA");

    u8g2Writer.setCursor(4, 24);
    u8g2Writer.print("5V: ");
    u8g2Writer.print(sensorData.voltage.fiveVolt.voltage);
    u8g2Writer.setCursor(4, 32);
    u8g2Writer.print("POWER: ");
    u8g2Writer.print(sensorData.voltage.fiveVolt.power / 1000);
    u8g2Writer.print(" W | ");
    u8g2Writer.print(sensorData.voltage.fiveVolt.current);
    u8g2Writer.print(" mA");
}

void Oled::displayLightInfo(DataStore &dataStore)
{
    if (strip != NULL) {
        String mode = strip->getModeName(dataStore.configData.argb.mode);
        u8g2Writer.setCursor(4, 8);
        u8g2Writer.print("Light Mode: ");
        u8g2Writer.print(mode);

        u8g2Writer.setCursor(4, 16);
        u8g2Writer.print("Brightness: ");
        u8g2Writer.print(strip->getBrightness());

        u8g2Writer.setCursor(4, 24);
        u8g2Writer.print("Color: ");
        // convert to hex
        u8g2Writer.print(strip->getColor(), HEX);

        u8g2Writer.setCursor(4, 32);
        u8g2Writer.print("Speed: ");
        u8g2Writer.print(strip->getSpeed());

        u8g2Writer.setCursor(4, 40);
        u8g2Writer.print("Source: ");
        if (dataStore.configData.argb.source == 0) {
            u8g2Writer.print("Controller");
        } else {
            u8g2Writer.print("External");
        }
    } else {
        u8g2Writer.setCursor(4, 8);
        u8g2Writer.print("Light Not Enabled");
    }
}
