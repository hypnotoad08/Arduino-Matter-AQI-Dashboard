#ifndef EXT4_SENSOR_READER_H
#define EXT4_SENSOR_READER_H
// PDLS
#include "PDLS_EXT4_Basic_Matter.h"

#if (PDLS_EXT4_BASIC_MATTER_RELEASE < 812)
#error Required PDLS_EXT4_BASIC_MATTER_RELEASE 812
#endif // PDLS_EXT4_BASIC_MATTER_RELEASE

// SDK
// #include <Arduino.h>
#include "hV_HAL_Peripherals.h"

// Configuration
#include "hV_Configuration.h"

#if (USE_EXT_BOARD != BOARD_EXT4)
#error Required USE_EXT_BOARD = BOARD_EXT4
#endif // USE_EXT_BOARD
// HDC2080
#include "Wire.h"

#define HDC_I2C 0x40

uint8_t bufferWrite[8] = { 0 };
uint8_t bufferRead[8] = { 0 };

struct Ext4SensorData {
    float temperatureC;
    float temperatureF;
    float humidityPercent;
};

void initExt4Sensor() {
    hV_HAL_begin();
    // --- HDC2080
    hV_HAL_Wire_begin();

    bufferWrite[0] = 0x0e; // HDC20X0_CONFIGURATION
    bufferWrite[1] = 0x00; // HDC20X0_DEFAULT_SETTINGS
    hV_HAL_Wire_transfer(HDC_I2C, bufferWrite, 2, bufferRead, 0);

    bufferWrite[0] = 0x0f; // HDC20X0_MEASURE_CONFIGURATION
    bufferWrite[1] = 0xfe; // HDC20X0_MEASURE_SETTINGS
    hV_HAL_Wire_transfer(HDC_I2C, bufferWrite, 2, bufferRead, 0);
    // --- End of HDC2080
}

Ext4SensorData readExt4SensorData() {
    Ext4SensorData data;

    // Trigger measurement
        bufferWrite[0] = 0x0f; // HDC20X0_MEASURE_CONFIGURATION
        bufferWrite[1] = 0xff; // HDC20X0_DEFAULT_SETTINGS
        hV_HAL_Wire_transfer(HDC_I2C, bufferWrite, 2, bufferRead, 0);
        delay(50);

    // Read temperature
    bufferWrite[0] = 0x00; // HDC20X0_TEMPERATURE_LOW
    hV_HAL_Wire_transfer(HDC_I2C, bufferWrite, 1, bufferRead, 2);
    uint16_t rawTemp = bufferRead[0] + bufferRead[1] * 256.0;
    data.temperatureC = ((rawTemp * 165.0 / 65536.0) - (40.5 + 0.08 * (3.3 - 1.8)));
    data.temperatureF = data.temperatureC * 9.0 / 5.0 + 32.0;

    // Read humidity
    bufferWrite[0] = 0x02; // HDC20X0_HUMIDITY_LOW
    hV_HAL_Wire_transfer(HDC_I2C, bufferWrite, 1, bufferRead, 2);
    uint16_t rawHumidity = bufferRead[0] + bufferRead[1] * 256.0;
    data.humidityPercent = rawHumidity / 65536.0 * 100.0;

    return data;
}

#endif // EXT4_SENSOR_READER_H
