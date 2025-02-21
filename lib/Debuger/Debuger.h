#include <Arduino.h>

#if defined(FANHUB_DEBUG) && !defined(USE_MODBUS)
// #define FANHUB_DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
// #define FANHUB_DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
// #define FANHUB_DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#define FANHUB_DEBUG_PRINT(...)
#define FANHUB_DEBUG_PRINTLN(...)
#define FANHUB_DEBUG_PRINTF(...)
#else
#define FANHUB_DEBUG_PRINT(...)
#define FANHUB_DEBUG_PRINTLN(...)
#define FANHUB_DEBUG_PRINTF(...)
#endif