#pragma once
#include <Arduino.h>

struct SensorReadings {
    int temperatureF;
    int humidityPercent;
    int aqi;
    int vocIndex;
    int co2ppm;
    int pm1;
    int pm25;
    int pm10;
    int screenTemperatureF;
    int screenHumidityPercent;
};

SensorReadings generateMockSensorData() {
    return {
        .temperatureF = random(65, 95),
        .humidityPercent = random(40, 90),
        .aqi = random(20, 160),
        .vocIndex = random(0, 500),
        .co2ppm = random(400, 1500),
        .pm1 = random(0, 100),
        .pm25 = random(0, 100),
        .pm10 = random(0, 100),
        .screenTemperatureF = random(65, 95),     // Mock screen sensor
        .screenHumidityPercent = random(40, 90)   // Mock screen sensor
    };
}

String generateMockTimestamp() {
    char buffer[22];
    snprintf(buffer, sizeof(buffer), "04/15 %02d:%02d:%02d", random(0, 24), random(0, 60), random(0, 60));
    return String(buffer);
}
