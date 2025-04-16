#pragma once
#include "SensorReadings.h"
#include "DrawingHelpers.h"
#include "WarningHelpers.h"
#include "PageTypes.h"
#include "DisplayManager.h"

extern Screen_EPD_EXT4_Fast myScreen;

void displaySensorPage(const SensorReadings& r, PageType page, const String& timestamp, int basePages, int totalPages) {
    const uint16_t xMax = myScreen.screenSizeX();
    const uint16_t yMax = myScreen.screenSizeY();
    const uint16_t dx = xMax / gridCols;
    const uint16_t dy = yMax / gridRows;
    const uint16_t x = dx;
    uint16_t y = dy;

    myScreen.setOrientation(3);
    myScreen.clear();
    myScreen.selectFont(Font_Terminal12x16);
    myScreen.gText(x, y, "Air Sensor Readings");
    y += 2 * dy;
    myScreen.selectFont(Font_Terminal8x12);

    if (page == ENV_CONDITIONS) {
        myScreen.gText(x, y, "Env Conditions"); y += 2 * dy;
        drawSensorWithBar("Temp (M)", r.temperatureF, 120, y, x, dx, dy, xMax);
        drawSensorWithBar("Temp (S)", r.screenTemperatureF, 120, y, x, dx, dy, xMax);
        drawSensorWithBar("Humidity (M)", r.humidityPercent, 100, y, x, dx, dy, xMax);
        drawSensorWithBar("Humidity (S)", r.screenHumidityPercent, 100, y, x, dx, dy, xMax);
        drawSensorWithBar("AQI", r.aqi, 200, y, x, dx, dy, xMax);
    } else if (page == PARTICULATES) {
        myScreen.gText(x, y, "Particulates"); y += 2 * dy;
        drawSensorWithBar("PM1", r.pm1, 200, y, x, dx, dy, xMax);
        drawSensorWithBar("PM2.5", r.pm25, 200, y, x, dx, dy, xMax);
        drawSensorWithBar("PM10", r.pm10, 200, y, x, dx, dy, xMax);
    } else if (page == GAS_AND_VOCS) {
        myScreen.gText(x, y, "Gas & VOCs"); y += 2 * dy;
        drawSensorWithBar("CO2", r.co2ppm, 2000, y, x, dx, dy, xMax);
        drawSensorWithBar("VOC Index", r.vocIndex, 500, y, x, dx, dy, xMax);
    } else if (page == WARNINGS) {
        myScreen.gText(x, y, "Warnings"); y += 1.5 * dy;
        drawWarnings(r, y, x, dx, dy);
    }

    myScreen.gText(x, yMax - dy, "Last update:");
    myScreen.gText(x + 6 * dx, yMax - dy, timestamp.c_str());

    char buf[20];
    snprintf(buf, sizeof(buf), "Page %d/%d", page + 1, totalPages);
    myScreen.gText(xMax - 6 * dx, yMax - dy, buf);

    DisplayManager::flush();
}
