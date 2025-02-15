#ifndef AT24CX_H
#define AT24CX_H

#include <Wire.h>

#define AT24CX_I2C_DEFAULT 0x50 // Default base address

class AT24CX {
public:
    AT24CX(TwoWire &wire = Wire, uint8_t i2c_addr = AT24CX_I2C_DEFAULT);
    bool begin();

    // Functions to write and read structs from EEPROM
    void writeStruct(uint16_t addr, const void *data, size_t size);
    void readStruct(uint16_t addr, void *data, size_t size);

private:
    TwoWire &_wire;
    uint8_t _i2c_addr;

    void writeByte(uint16_t addr, uint8_t data);
    uint8_t readByte(uint16_t addr);
    bool testConnection();
};

#endif