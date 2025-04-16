#pragma once
#include "PDLS_EXT4_Basic_Matter.h"
#include "SensorReadings.h"

extern Screen_EPD_EXT4_Fast myScreen;

inline bool hasHighTemp(const SensorReadings& r) {
  return r.temperatureF > 85;
}
inline bool hasHighHumidity(const SensorReadings& r) {
  return r.humidityPercent > 70;
}
inline bool hasPoorAQI(const SensorReadings& r) {
  return r.aqi > 100;
}

int getWarningCount(const SensorReadings& r) {
  return hasHighTemp(r) + hasHighHumidity(r) + hasPoorAQI(r);
}

void drawHeatWarningIcon(uint16_t x, uint16_t y, uint16_t size) {
  myScreen.setPenSolid(true);
  for (int i = 0; i < 3; i++) myScreen.dLine(x, y + i * 4, size, 0, myColours.black);
}

void drawHumidityIcon(uint16_t x, uint16_t y, uint16_t size) {
  myScreen.circle(x + size / 2, y + size / 2, size / 4, myColours.black);
  myScreen.point(x + size / 2, y + size / 2 + 2, myColours.black);
}

void drawAQIIcon(uint16_t x, uint16_t y, uint16_t size) {
  uint16_t midX = x + size / 2;
  myScreen.triangle(midX, y, x, y + size, x + size, y + size, myColours.black);
  myScreen.setPenSolid(false);
  myScreen.circle(midX, y + size * 2 / 3, size / 10, myColours.black);
}

void drawWarningLine(const char* message, void (*iconFunc)(uint16_t, uint16_t, uint16_t),
                     uint16_t& y, uint16_t dx, uint16_t dy, uint16_t iconX) {
  myScreen.gText(dx, y, message);
  iconFunc(iconX, y + iconYOffset, defaultIconSize);
  y += dy;
}

void drawWarnings(const SensorReadings& r, uint16_t& y, uint16_t x, uint16_t dx, uint16_t dy) {
  uint16_t warningStartY = y;
  const uint16_t iconX = x + 12 * dx;
  int count = 0;

  if (hasHighTemp(r)) drawWarningLine("Warning: High Temp!", drawHeatWarningIcon, y, x, dy, iconX), ++count;
  if (hasHighHumidity(r)) drawWarningLine("Warning: High Humidity!", drawHumidityIcon, y, x, dy, iconX), ++count;
  if (hasPoorAQI(r)) drawWarningLine("Warning: Poor AQI!", drawAQIIcon, y, x, dy, iconX), ++count;

  if (count > 0) {
    myScreen.setPenSolid(false);
    myScreen.dRectangle(x - 4, warningStartY - 2, 17.3 * dx, count * dy + 6, myColours.black);
  }
}
