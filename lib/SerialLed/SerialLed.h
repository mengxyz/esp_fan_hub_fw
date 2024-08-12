#if !defined(SerialLed_h)
#define SerialLed_h

#include <WS2812FX.h>

#define LED_COUNT 40
#define PACKET_SZ ( (LED_COUNT * 3) + 3 )

class SerialLed
{
private:
    unsigned char serial_buffer[PACKET_SZ];
    unsigned int head = 0;
    unsigned int start;
    unsigned int checksum_1;
    unsigned int checksum_0;
public:
    void service(WS2812FX &argb);
    void service2(Adafruit_NeoPixel &argb);
};

#endif // SerialLed_h
