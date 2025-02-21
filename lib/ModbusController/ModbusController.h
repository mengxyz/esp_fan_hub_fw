#if !defined(MODBUS_CONTROLLER_H)
#define MODBUS_CONTROLLER_H

#include <DataStore.h>
#include <FanControl.h>
#include <ModbusRTUSlave.h>
#include <SwitchSource.h>

const uint8_t modbus_buad_rate_count = 11;
const uint32_t modbus_baud_rates[modbus_buad_rate_count] = {
    1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, 76800, 115200};

#define MODBUS_DERIAL Serial0
#define MODBUS_CONFIG SERIAL_8N1
#define MODBUS_MAX_COILS 6
#define MODBUS_MAX_HOLDING_REGISTERS 26
// Addresses
#define MODBUS_HUMIDITY_ADDRESS 0
#define MODBUS_TEMPERATURE_ADDRESS 1
#define MODBUS_5V_VOLTAGE_ADDRESS 2
#define MODBUS_12V_VOLTAGE_ADDRESS 3
#define MODBUS_5V_POWER_ADDRESS 4
#define MODBUS_12V_POWER_ADDRESS 5
#define MODBUS_5V_CURRENT_ADDRESS 6
#define MODBUS_12V_CURRENT_ADDRESS 7
#define MODBUS_THERMISTER_1_ADDRESS 8
#define MODBUS_THERMISTER_2_ADDRESS 9
#define MODBUS_WATER_FLOW_ADDRESS 10
#define MODBUS_WIFI_SSID_ADDRESS 11
#define MODBUS_WIFI_PASSWORD_ADDRESS 12
#define MODBUS_WIFI_MDNS_ADDRESS 13

#define MODBUS_FAN_DUTY_CH1_ADDRESS 14
#define MODBUS_FAN_DUTY_CH2_ADDRESS 15
#define MODBUS_FAN_DUTY_CH3_ADDRESS 16
#define MODBUS_FAN_DUTY_CH4_ADDRESS 17
#define MODBUS_FAN_DUTY_CH5_ADDRESS 18

#define MODBUS_FAN_SPEED_CH1_ADDRESS 19
#define MODBUS_FAN_SPEED_CH2_ADDRESS 20
#define MODBUS_FAN_SPEED_CH3_ADDRESS 21
#define MODBUS_FAN_SPEED_CH4_ADDRESS 22
#define MODBUS_FAN_SPEED_CH5_ADDRESS 23

// Settings
#define MODBUS_UNIT_ID_ADDRESS 24
#define MODBUS_BAUD_RATE_ADDRESS 25

class ModbusController {

private:
  DataStore *dataStore;
  SwitchSource *swSource;
  FanControl *fanControl;
  ModbusRTUSlave modbus = ModbusRTUSlave(MODBUS_DERIAL);
  byte isChange = false;
  uint16_t holdingRegisters[MODBUS_MAX_HOLDING_REGISTERS];
  bool coils[MODBUS_MAX_COILS];

public:
  ModbusController(DataStore *dataStore, SwitchSource *swSource,
                   FanControl *fanControl);
  void service();
  void begin();
  void onChangeUnitId();
  void handleReadonlyHoldingRegisters();
  static uint16_t convertUnit(uint16_t &value, uint8_t unit) {
    return (uint16_t)(value * unit);
  }
  static uint16_t convertUnit(float &value, uint8_t unit) {
    return (uint16_t)(value * unit);
  }
};

#endif