#pragma once
#include "PDLS_EXT4_Basic_Matter.h"

extern Screen_EPD_EXT4_Fast myScreen;

const uint16_t defaultIconSize = 12;
const uint16_t iconYOffset = 2;
const uint16_t gridCols = 20;
const uint16_t gridRows = 12;

const char* getUnitForLabel(const char* label) {
    if (strstr(label, "Temp")) return " F";
    if (strstr(label, "Humidity")) return " %";
    if (strcmp(label, "CO2") == 0) return " ppm";
    if (strstr(label, "PM")) return " ug/m3";
    return "";
}

void drawSensorBar(uint16_t x, uint16_t y, uint16_t width, uint16_t height, int value, int maxValue) {
    value = constrain(value, 0, maxValue);
    uint16_t filled = map(value, 0, maxValue, 0, width);
    myScreen.setPenSolid(true);
    myScreen.dRectangle(x, y, filled, height, myColours.black);
    myScreen.setPenSolid(false);
    myScreen.dRectangle(x, y, width, height, myColours.black);
}

void drawSensorWithBar(const char* label, int value, int maxValue, uint16_t& y,
                       uint16_t x, uint16_t dx, uint16_t dy, uint16_t xMax) {
    const uint16_t valueX = x + 6 * dx;
    const uint16_t barX = x + 12 * dx;
    const uint16_t barWidth = xMax - barX - dx;
    myScreen.gText(x, y, label);
    myScreen.gText(valueX, y, String(value) + getUnitForLabel(label));
    drawSensorBar(barX, y, barWidth, dy - 4, value, maxValue);
    y += dy;
}
