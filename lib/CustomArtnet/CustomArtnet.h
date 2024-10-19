// const int numberOfChannels = NUM_LEDS * 3; // Total number of channels you want to receive (1 led = 3 channels)
// ArtnetWifi artnet;
// const int startUniverse = 0;
// Adafruit_NeoPixel leds = Adafruit_NeoPixel(NUM_LEDS, ARGB_PIN, NEO_GRB + NEO_KHZ800);
// const int maxUniverses = numberOfChannels / 512 + ((numberOfChannels % 512) ? 1 : 0);
// bool universesReceived[maxUniverses];
// bool sendFrame = 1;
// int previousDataLength = 0;
// void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t *data)
// {
//   DEBUG_PRINTF("onDmxFrame U: %d L: %d S: %d\n", universe, length, sequence);
//   sendFrame = 1;
//   // set brightness of the whole strip
//   if (universe == 15)
//   {
//     leds.setBrightness(data[0]);
//     leds.show();
//   }

//   // Store which universe has got in
//   if ((universe - startUniverse) < maxUniverses)
//     universesReceived[universe - startUniverse] = 1;

//   for (int i = 0; i < maxUniverses; i++)
//   {
//     if (universesReceived[i] == 0)
//     {
//       DEBUG_PRINTLN("Broke");
//       sendFrame = 0;
//       break;
//     }
//   }

//   // read universe and put into the right part of the display buffer
//   for (int i = 0; i < length / 3; i++)
//   {
//     int led = i + (universe - startUniverse) * (previousDataLength / 3);
//     if (led < NUM_LEDS)
//       leds.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
//   }
//   previousDataLength = length;

//   if (sendFrame)
//   {
//     leds.show();
//     // Reset universeReceived to 0
//     memset(universesReceived, 0, maxUniverses);
//   }
// }

#if !defined(CUSTOM_ARTNET_H)
#define CUSTOM_ARTNET_H



#endif // CUSTOM_ARTNET_H
