#include "Adalight.h"

const uint8_t Adalight::magic[] = {'A', 'd', 'a'};
const uint8_t Adalight::MAGICSIZE = sizeof(magic);
const uint8_t Adalight::HICHECK = MAGICSIZE;
const uint8_t Adalight::LOCHECK = MAGICSIZE + 1;
const uint8_t Adalight::CHECKSUM = MAGICSIZE + 2;

Adalight::Adalight(uint16_t numLeds, uint8_t pin, uint8_t brightness)
    : Num_Leds(numLeds), Pin(pin), Brightness(brightness),
      strip(Adafruit_NeoPixel(numLeds, pin, NEO_GRB + NEO_KHZ800)) {
    mode = Header;
}

void Adalight::begin() {
    strip.begin();
    strip.setBrightness(Brightness);
    strip.show(); // Initialize all pixels to 'off'

    if (isSerialReady()) {
        FANHUB_DEBUG_PRINT("Ada\n");
    }

    lastByteTime = lastAckTime = millis();
}

void Adalight::process() {
    if (!isSerialReady()) return;

    t = millis();

    if ((c = Serial.read()) >= 0) {
        lastByteTime = lastAckTime = t;

        switch (mode) {
            case Header:
                headerMode();
                break;
            case Data:
                dataMode();
                break;
        }
    } else {
        timeouts();
    }
}

void Adalight::headerMode() {
    static uint8_t headPos, hi, lo, chk;

    if (headPos < MAGICSIZE) {
        if (c == magic[headPos]) {
            headPos++;
        } else {
            headPos = 0;
        }
    } else {
        switch (headPos) {
            case HICHECK:
                hi = c;
                headPos++;
                break;
            case LOCHECK:
                lo = c;
                headPos++;
                break;
            case CHECKSUM:
                chk = c;
                if (chk == (hi ^ lo ^ 0x55)) {
                    bytesRemaining = 3L * (256L * (long)hi + (long)lo + 1L);
                    outPos = 0;
                    strip.clear();
                    mode = Data;
                }
                headPos = 0;
                break;
        }
    }
}

void Adalight::dataMode() {
    if (outPos < Num_Leds * 3) {
        if (outPos % 3 == 0) {
            strip.setPixelColor(outPos / 3, c, 0, 0);
        } else if (outPos % 3 == 1) {
            uint32_t color = strip.getPixelColor(outPos / 3);
            strip.setPixelColor(outPos / 3, (color & 0xFF0000) >> 16, c, 0);
        } else {
            uint32_t color = strip.getPixelColor(outPos / 3);
            strip.setPixelColor(outPos / 3, (color & 0xFF0000) >> 16, (color & 0x00FF00) >> 8, c);
        }
        outPos++;
    }
    bytesRemaining--;

    if (bytesRemaining == 0) {
        mode = Header;
        strip.show();
    }
}

void Adalight::timeouts() {
    if ((t - lastAckTime) >= 1000) {
        FANHUB_DEBUG_PRINT("Ada\n");
        lastAckTime = t;

        if ((t - lastByteTime) >= 60000) {
            strip.clear();
            strip.show();
            mode = Header;
            lastByteTime = t;
        }
    }
}

bool Adalight::isSerialReady() {
    return Serial;
}