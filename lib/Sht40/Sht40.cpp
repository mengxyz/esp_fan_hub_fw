#include <Sht40.h>

SensirionI2cSht4x sht;
void initSht40()
{
    char errorMessage[64];
    int16_t error;
    sht.begin(Wire, SHT40_I2C_ADDR_44);
    sht.softReset();
    delay(10);
    uint32_t serialNumber = 0;
    error = sht.serialNumber(serialNumber);
    if (error != NO_ERROR)
    {
        Serial.println("SHT40 not ready");
        return;
    }
    Serial.println("SHT40 ready");
    Serial.printf("SHT40 sn %d \n", serialNumber);
}