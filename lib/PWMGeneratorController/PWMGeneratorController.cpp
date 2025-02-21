#include "PWMGeneratorController.h"

#define SET_DUTY_CH1_CMD 0x10
#define SET_DUTY_CH2_CMD 0x11
#define SET_DUTY_CH3_CMD 0x12
#define SET_DUTY_CH4_CMD 0x13
#define SET_DUTY_CH5_CMD 0x14

#define BEGIN_PWM_CMD 0x01
#define DISABLE_PWM_CMD 0x05
#define ENABLE_PWM_CMD 0x06

const uint8_t SET_DUTY_COMMANDS[5] = {SET_DUTY_CH1_CMD, SET_DUTY_CH2_CMD, SET_DUTY_CH3_CMD, SET_DUTY_CH4_CMD, SET_DUTY_CH5_CMD};

#define SET_FREQ_CMD 0x02
#define READ_DUTY_CMD 0x03
#define READ_FREQ_CMD 0x04
#define MAX_CHANNEL 5

PWMGeneratorController::PWMGeneratorController(uint8_t slaveAddr, TwoWire* wire)
    : _slaveAddr(slaveAddr), _wire(wire) {}

bool PWMGeneratorController::begin() {
    _wire->beginTransmission(_slaveAddr);
    _wire->write(READ_FREQ_CMD); // Command to get frequency
    _wire->endTransmission();

    // Check if the PWM generator is ready by sending a dummy command
    _wire->beginTransmission(_slaveAddr);
    uint8_t error = _wire->endTransmission();

    if (error == 0) {
        Serial.println("PWMGeneratorController initialized successfully.");
        return true; // Success
    } else {
        Serial.printf("Error initializing PWMGeneratorController: I2C error code %d\n", error);
        return false; // Failure
    }
}

uint8_t PWMGeneratorController::getDuty(uint8_t channel) {
    if (channel < 0 || channel > 4) {
        Serial.println("Error: Invalid channel.");
        return -1; // Error: Invalid channel
    }

    sendCommand(READ_DUTY_CMD);

    _wire->requestFrom(_slaveAddr, MAX_CHANNEL);
    
    uint8_t i = 0;
    while (_wire->available() && i < channel) {
        _readBuff[i] = _wire->read();
        i++;
    }
    // convert back from percent to 0 - 255
    return ceil(_readBuff[channel] * 255 / 100);
}

void PWMGeneratorController::setDuty(uint8_t channel, uint8_t duty) {
    uint8_t dutyPercentage = ceil(duty / 255.0 * 100);
    Serial.printf("Channel: %d, Duty: %d%%\n", channel, dutyPercentage);
    if (channel < 0 || channel > 4 || dutyPercentage < 0 || dutyPercentage > 100) {
        Serial.printf("Error: Invalid channel or duty cycle. %d, %d\n", channel, dutyPercentage);
        return;
    }

    sendCommandData(SET_DUTY_COMMANDS[channel], dutyPercentage);
}

uint8_t PWMGeneratorController::getFreq() {
    sendCommand(READ_FREQ_CMD);

    // Request the response
    _wire->requestFrom(_slaveAddr, 1);
    if (_wire->available()) {
        return _wire->read() * 1000; // Frequency in Hz
    }

    return -1; // Error: No data received
}

void PWMGeneratorController::setFreq(uint8_t freq) {
    if (freq <= 0) {
        Serial.println("Error: Invalid frequency.");
        return;
    }
    sendCommandData(SET_FREQ_CMD, freq / 1000);
}

void PWMGeneratorController::init() {
    sendCommand(BEGIN_PWM_CMD);
}

void PWMGeneratorController::disable() {
    sendCommand(DISABLE_PWM_CMD);
}

void PWMGeneratorController::enable() {
    sendCommand(ENABLE_PWM_CMD);
}

void PWMGeneratorController::sendCommandData(uint8_t command, uint8_t data)
{
    _wire->beginTransmission(_slaveAddr);
    _wire->write(command);
    _wire->write(data);
    _wire->endTransmission();
}

void PWMGeneratorController::sendCommand(uint8_t command)
{
    _wire->beginTransmission(_slaveAddr);
    _wire->write(command);
    _wire->endTransmission();
}
