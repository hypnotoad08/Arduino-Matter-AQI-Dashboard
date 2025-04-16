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
};

SensorReadings generateMockSensorData() {
    return {
        random(65, 95), random(40, 90), random(20, 160), random(0, 500),
        random(400, 1500), random(0, 100), random(0, 100), random(0, 100)
    };
}

String generateMockTimestamp() {
    char buffer[22];
    snprintf(buffer, sizeof(buffer), "04/15 %02d:%02d:%02d", random(0, 24), random(0, 60), random(0, 60));
    return String(buffer);
}
