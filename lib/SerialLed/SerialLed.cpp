#include "SerialLed.h"

void SerialLed::service(WS2812FX &argb)
{
    if (Serial.available())
    {
        serial_buffer[head] = Serial.read();

        if (head >= (PACKET_SZ - 1))
        {
            start = 0;
            checksum_1 = head;
            checksum_0 = head - 1;
            head = 0;
        }
        else
        {
            start = head + 1;
            checksum_1 = head;
            if (head == 0)
            {
                checksum_0 = PACKET_SZ - 1;
            }
            else
            {
                checksum_0 = head - 1;
            }
            head++;
        }

        if (serial_buffer[start] == 0xAA)
        {
            unsigned short sum = 0;

            for (int i = 0; i < checksum_0; i++)
            {
                sum += serial_buffer[i];
            }

            if (start > 0)
            {
                for (int i = start; i < PACKET_SZ; i++)
                {
                    sum += serial_buffer[i];
                }
            }

            // Test if valid write packet
            if ((((unsigned short)serial_buffer[checksum_0] << 8) | serial_buffer[checksum_1]) == sum)
            {
                noInterrupts();
                for (int i = 0; i < LED_COUNT; i++)
                {
                    int idx = start + 1 + (3 * i);

                    if (idx >= (PACKET_SZ - 1))
                    {
                        idx = idx - PACKET_SZ;
                    }
                    argb.setPixelColor(i, serial_buffer[idx], serial_buffer[idx + 1], serial_buffer[idx + 2]);
                }
                argb.show();
                interrupts();
            }
        }
    }
}

void SerialLed::service2(Adafruit_NeoPixel &argb)
{
    if (Serial.available())
    {
        serial_buffer[head] = Serial.read();

        if (head >= (PACKET_SZ - 1))
        {
            start = 0;
            checksum_1 = head;
            checksum_0 = head - 1;
            head = 0;
        }
        else
        {
            start = head + 1;
            checksum_1 = head;
            if (head == 0)
            {
                checksum_0 = PACKET_SZ - 1;
            }
            else
            {
                checksum_0 = head - 1;
            }
            head++;
        }

        if (serial_buffer[start] == 0xAA)
        {
            unsigned short sum = 0;

            for (int i = 0; i < checksum_0; i++)
            {
                sum += serial_buffer[i];
            }

            if (start > 0)
            {
                for (int i = start; i < PACKET_SZ; i++)
                {
                    sum += serial_buffer[i];
                }
            }

            // Test if valid write packet
            if ((((unsigned short)serial_buffer[checksum_0] << 8) | serial_buffer[checksum_1]) == sum)
            {
                // noInterrupts();
                for (int i = 0; i < LED_COUNT; i++)
                {
                    int idx = start + 1 + (3 * i);

                    if (idx >= (PACKET_SZ - 1))
                    {
                        idx = idx - PACKET_SZ;
                    }
                    // argb.setPixelColor(i, serial_buffer[idx], serial_buffer[idx + 1], serial_buffer[idx + 2]);
                }
                // argb.show();
                // interrupts();
            }
        }

        argb.setPixelColor(0, 255, 0, 0);
        argb.show();
        delay(10);
        argb.setPixelColor(0, 0, 0, 0);
        argb.show();

    }
}