#ifdef FANHUB_DEBUG
#define DEBUG_PRINT(...) DEBUG_PRINT(__VA_ARGS__)
#define DEBUG_PRINTLN(...) DEBUG_PRINTLN(__VA_ARGS__)
#define DEBUG_PRINTF(...) DEBUG_PRINTLN(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#define DEBUG_PRINTF(...)
#endif