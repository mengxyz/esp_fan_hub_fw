#define BTN_UTIL_PIN_3 17 // ARGB RND
#define EXTERNAL_INTERUPT_PIN 45
#define ARGB_PIN 8
#define ARGB_DEBUG_PIN 3
#define DEBUG_LED_NUM 1
#define COLOR_ORDER GRB
#define CHIPSET WS2811
#define NUM_LEDS 40
#define BRIGHTNESS 200
#define FRAMES_PER_SECOND 60

// I2C device found at address 0x21  !
// I2C device found at address 0x40  !
// I2C device found at address 0x41  !
// I2C device found at address 0x44  !
// I2C device found at address 0x50  !

#define PCF8574_I2C_ADDRESS 0x21
// 0x40: A0 = GND, A1 = GND
// 0x41: A0 = VCC, A1 = GND
// 0x44: A0 = GND, A1 = VCC
// 0x45: A0 = VCC, A1 = VCC
// 0x48: A0 = GND, A1 = SDA
// 0x49: A0 = VCC, A1 = SDA
// 0x4C: A0 = GND, A1 = SCL
// 0x4D: A0 = VCC, A1 = SCL
#define INA219_5V_I2C_ADDRESS 0x40  // A1 = GND, A0 = GND
#define INA219_12V_I2C_ADDRESS 0x41 // A1 = GND, A0 = VCC
#define INA3221_MOLEX_5V_I2C_ADDRESS 0x43
#define INA3221_MOLEX_12_I2C_ADDRESS 0x42
#define SHT30_I2C_ADDRESS 0x44
#define ADS1115_I2C_ADDRESS 0x48
// #define ADS1115_I2C_ADDRESS 0x49
#define EEPROM_I2C_ADDRESS 0x50
#define DS3231_I2C_ADDRESS 0x68
#ifdef USE_MODBUS
#define SERIAL_BAUD_RATE 9600
#else
#define SERIAL_BAUD_RATE 115200
#endif
// #define SCL_PIN 11
// #define SDA_PIN 12
#define SCL_PIN 14
#define SDA_PIN 13
#define EEPROM_RESET_PIN 12
#define PWM_CONTROL_ADDR 0x71
#define MODBUS_UNIT_ID 1
#define MODBUS_BAUD_RATE_INDEX 3