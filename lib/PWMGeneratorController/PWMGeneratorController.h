#ifndef PWMGENERATORCONTROLLER_H
#define PWMGENERATORCONTROLLER_H

#include <Arduino.h>
#include <Wire.h>

#define PWM_GENERATOR_I2C_ADDRESS 0x71

class PWMGeneratorController {
public:
    PWMGeneratorController(uint8_t slaveAddr, TwoWire* wire = &Wire);

    bool begin(); // Initialize the I2C communication
    uint8_t getDuty(uint8_t channel); // Get the duty cycle of a specific channel
    void setDuty(uint8_t channel, uint8_t duty); // Set the duty cycle of a specific channel
    uint8_t getFreq(); // Get the frequency of the PWM generator
    void setFreq(uint8_t freq); // Set the frequency of the PWM generator
    void init();
    void disable();
    void enable();

private:
    uint8_t _slaveAddr; // I2C address of the PWM generator
    TwoWire* _wire; // Pointer to the Wire object (Wire or Wire1)
    void sendCommandData(uint8_t command, uint8_t data);
    void sendCommand(uint8_t command);
    uint8_t _readBuff[5];
};

#endif