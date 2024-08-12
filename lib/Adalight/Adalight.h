#ifndef ADALIGHT_H
#define ADALIGHT_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class Adalight {
public:
    Adalight(uint16_t numLeds, uint8_t pin, uint8_t brightness);
    void begin();
    void process();

private:
    // Constants
    static const uint8_t magic[];
    static const uint8_t MAGICSIZE;
    static const uint8_t HICHECK;
    static const uint8_t LOCHECK;
    static const uint8_t CHECKSUM;

    // Settings
    const uint16_t Num_Leds;
    const uint8_t Pin;
    const uint8_t Brightness;
    Adafruit_NeoPixel strip;

    // State variables
    enum processModes_t {Header, Data} mode;
    int16_t c;
    uint16_t outPos;
    uint32_t bytesRemaining;
    unsigned long t, lastByteTime, lastAckTime;

    // Methods
    void headerMode();
    void dataMode();
    void timeouts();
    bool isSerialReady();
};

#endif // ADALIGHT_H