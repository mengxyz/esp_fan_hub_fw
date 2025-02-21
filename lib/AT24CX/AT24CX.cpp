#include "AT24CX.h"

AT24CX::AT24CX(TwoWire &wire, uint8_t i2c_addr) : _wire(wire), _i2c_addr(i2c_addr) {}

bool AT24CX::begin() {
    _wire.begin();
    return testConnection();
}

void AT24CX::writeByte(uint16_t addr, uint8_t data) {
    _wire.beginTransmission(_i2c_addr | ((addr >> 8) & 0x07));
    _wire.write((uint8_t)(addr & 0xFF)); // 8-bit memory address
    _wire.write(data);
    _wire.endTransmission();
    delay(20); // EEPROM write cycle delay
}

uint8_t AT24CX::readByte(uint16_t addr) {
    _wire.beginTransmission(_i2c_addr | ((addr >> 8) & 0x07));
    _wire.write((uint8_t)(addr & 0xFF));
    _wire.endTransmission();

    _wire.requestFrom((uint8_t)(_i2c_addr | ((addr >> 8) & 0x07)), (uint8_t)1);
    return _wire.available() ? _wire.read() : 0xFF;
}

bool AT24CX::testConnection() {
    _wire.beginTransmission(_i2c_addr);
    return (_wire.endTransmission() == 0);
}

// Write a struct or any data type to EEPROM
void AT24CX::writeStruct(uint16_t addr, const void *data, size_t size) {
    const uint8_t *byteData = static_cast<const uint8_t *>(data);
    for (size_t i = 0; i < size; ++i) {
        writeByte(addr + i, byteData[i]);
    }
}

// Read a struct or any data type from EEPROM
void AT24CX::readStruct(uint16_t addr, void *data, size_t size) {
    uint8_t *byteData = static_cast<uint8_t *>(data);
    for (size_t i = 0; i < size; ++i) {
        byteData[i] = readByte(addr + i);
    }
}