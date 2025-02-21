#include "ModbusController.h"

ModbusController::ModbusController(DataStore *dataStore, SwitchSource *swSource,
                                   FanControl *fanControl) {
  this->dataStore = dataStore;
  this->swSource = swSource;
  this->fanControl = fanControl;
}

void ModbusController::service() {
  handleReadonlyHoldingRegisters();

  modbus.poll();

  for (int i = 0; i < MODBUS_MAX_COILS - 1; i++) {
    if (dataStore->configData.fanSource[i] != coils[i]) {
      dataStore->configData.fanSource[i] = coils[i];
      swSource->setSource(FAN_INPUT_SOURCES[i], coils[i]);
      isChange = true;
    }
  }
  if (dataStore->configData.argb.source != coils[MODBUS_MAX_COILS - 1]) {
    swSource->setSource(InputSource::ARGB_SW, coils[MODBUS_MAX_COILS - 1]);
    dataStore->configData.argb.source = coils[MODBUS_MAX_COILS - 1];
    isChange = true;
  }

  if (holdingRegisters[MODBUS_UNIT_ID_ADDRESS] !=
      dataStore->configData.modbus.unit_id) {
    dataStore->configData.modbus.unit_id =
        holdingRegisters[MODBUS_UNIT_ID_ADDRESS];
    onChangeUnitId();
    isChange = true;
  }

  if (holdingRegisters[MODBUS_BAUD_RATE_ADDRESS] !=
      dataStore->configData.modbus.baud_rate_index) {
    dataStore->configData.modbus.baud_rate_index =
        holdingRegisters[MODBUS_BAUD_RATE_ADDRESS];
    onChangeUnitId();
    isChange = true;
  }

  if (isChange) {
    dataStore->saveConfigData();
    isChange = false;
  }
}

void ModbusController::begin() {
  modbus.configureCoils(coils, MODBUS_MAX_COILS);
  modbus.configureHoldingRegisters(holdingRegisters,
                                   MODBUS_MAX_HOLDING_REGISTERS);
  MODBUS_DERIAL.begin(
      modbus_baud_rates[dataStore->configData.modbus.baud_rate_index],
      MODBUS_CONFIG);
  modbus.begin(dataStore->configData.modbus.unit_id,
               modbus_baud_rates[dataStore->configData.modbus.baud_rate_index],
               MODBUS_CONFIG);

  for (int i = 0; i < MODBUS_MAX_COILS - 1; i++) {
    coils[i] = dataStore->configData.fanSource[i];
  }
  coils[MODBUS_MAX_COILS - 1] = dataStore->configData.argb.source;
}

void ModbusController::onChangeUnitId() {
  MODBUS_DERIAL.flush();
  MODBUS_DERIAL.updateBaudRate(
      modbus_baud_rates[dataStore->configData.modbus.baud_rate_index]);
  modbus.begin(dataStore->configData.modbus.unit_id,
               modbus_baud_rates[dataStore->configData.modbus.baud_rate_index],
               MODBUS_CONFIG);
}

void ModbusController::handleReadonlyHoldingRegisters() {
  holdingRegisters[MODBUS_TEMPERATURE_ADDRESS] =
      convertUnit(dataStore->sensorData.boardTemp.temp, 100);
  holdingRegisters[MODBUS_HUMIDITY_ADDRESS] =
      convertUnit(dataStore->sensorData.boardTemp.humi, 100);

  holdingRegisters[MODBUS_5V_VOLTAGE_ADDRESS] =
      convertUnit(dataStore->sensorData.voltage.fiveVolt.voltage, 100);
  holdingRegisters[MODBUS_12V_VOLTAGE_ADDRESS] =
      convertUnit(dataStore->sensorData.voltage.twelveVolt.voltage, 100);
  holdingRegisters[MODBUS_5V_POWER_ADDRESS] =
      convertUnit(dataStore->sensorData.voltage.fiveVolt.power, 100);
  holdingRegisters[MODBUS_12V_POWER_ADDRESS] =
      convertUnit(dataStore->sensorData.voltage.twelveVolt.power, 100);
  holdingRegisters[MODBUS_5V_CURRENT_ADDRESS] =
      convertUnit(dataStore->sensorData.voltage.fiveVolt.current, 100);
  holdingRegisters[MODBUS_12V_CURRENT_ADDRESS] =
      convertUnit(dataStore->sensorData.voltage.twelveVolt.current, 100);
  holdingRegisters[MODBUS_THERMISTER_1_ADDRESS] =
      convertUnit(dataStore->sensorData.thermistor.ch0Temp, 100);

  for (int i = MODBUS_FAN_SPEED_CH1_ADDRESS;
       i < MODBUS_FAN_SPEED_CH5_ADDRESS; i++) {
    holdingRegisters[i] =
        dataStore->sensorData.fanData.rpm[i - MODBUS_FAN_SPEED_CH1_ADDRESS];
  }
}
