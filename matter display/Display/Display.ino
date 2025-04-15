
#include "PDLS_EXT4_Basic_Matter.h"
#include "hV_HAL_Peripherals.h"

Screen_EPD_EXT4_Fast myScreen(eScreen_EPD_290_KS_0F, boardArduinoNanoMatter);

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

const uint16_t iconSize = 12;
const uint16_t iconYOffset = 2; 
const uint16_t gridCols = 20;
const uint16_t gridRows = 12;

const unsigned long pageDuration = 8000; // 8 seconds per page
unsigned long lastPageSwitch = 0;
int currentPage = 0;
const int totalPages = 3;
SensorReadings currentReadings;


struct SensorField {
    const char* label;
    int value;
    const char* unit;
    int maxValue;
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
    };
}


void drawHeatIcon(uint16_t x, uint16_t y, uint16_t size) {
    myScreen.dLine(x, y, size, 0, myColours.black);
    myScreen.dLine(x + 2, y + 4, size, 0, myColours.black);
    myScreen.dLine(x + 4, y + 8, size, 0, myColours.black);
}

void drawHeatWarningIcon(uint16_t x, uint16_t y, uint16_t size)
{
    myScreen.setPenSolid(true);
    for (int i = 0; i < 3; i++) {
        myScreen.dLine(x, y + i * 4, size, 0, myColours.black);
    }
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

void drawSensorBar(uint16_t x, uint16_t y, uint16_t width, uint16_t height, int value, int maxValue) {
    value = constrain(value, 0, maxValue);
    uint16_t filled = map(value, 0, maxValue, 0, width);
    myScreen.setPenSolid(true);
    myScreen.dRectangle(x, y, filled, height, myColours.black);
    myScreen.setPenSolid(false);
    myScreen.dRectangle(x, y, width, height, myColours.black);
}

void drawSensorWithBar(const char* label, int value, int maxValue, uint16_t& y,
                       uint16_t x, uint16_t barX, uint16_t dx, uint16_t dy)
{
    myScreen.gText(x, y, formatString("%s: %d%s", label, value, 
        strcmp(label, "Humidity") == 0 ? "%" : strcmp(label, "Temp") == 0 ? " F" : ""));
    drawSensorBar(barX, y, 10 * dx, dy - 4, value, maxValue);
    y += dy;
}

bool hasHighTemp(const SensorReadings& r)     { return r.temperatureF > 85; }
bool hasHighHumidity(const SensorReadings& r) { return r.humidityPercent > 70; }
bool hasPoorAQI(const SensorReadings& r)      { return r.aqi > 100; }

int getWarningCount(const SensorReadings& r) {
    int count = 0;
    if(hasHighTemp(r)) count++;
    if(hasHighHumidity(r)) count++;
    if(hasPoorAQI(r)) count++;
    return count;
}

void drawWarningLine(const char* const message, void (*iconFunc)(uint16_t, uint16_t, uint16_t),
                     uint16_t& y, uint16_t dx, uint16_t dy, uint16_t iconX)

{
    myScreen.gText(dx, y, message);
    iconFunc(iconX, y + iconYOffset, iconSize);
    y += dy;
}

void displaySensorData(const SensorReadings& readings) {
    myScreen.setOrientation(3);
    myScreen.clear();

    uint16_t xMax = myScreen.screenSizeX();
    uint16_t yMax = myScreen.screenSizeY();
    const uint16_t dx = xMax / gridCols;
    const uint16_t dy = yMax / gridRows;
    uint16_t x = dx;
    uint16_t y = dy;
    uint16_t sensorBarX = x + 7 * dx;

    myScreen.selectFont(Font_Terminal12x16);
    myScreen.gText(x, y, "Air Sensor Readings");
    y += 2 * dy;
    myScreen.selectFont(Font_Terminal8x12);

    // Display sensor values
    auto drawSensorWithBar = [&](const char* label, int value, int max) {
        myScreen.gText(x, y, formatString("%s: %d", label, value));
        drawSensorBar(sensorBarX, y, 10 * dx, dy - 4, value, max);
        y += dy;
    };

    drawSensorWithBar("Temp (F)", readings.temperatureF, 120);
    drawSensorWithBar("Humidity (%)", readings.humidityPercent, 100);
    drawSensorWithBar("AQI", readings.aqi, 200);
    drawSensorWithBar("PM1", readings.pm1, 200);
    drawSensorWithBar("PM2.5", readings.pm25, 200);
    drawSensorWithBar("PM10",readings.pm10,200);
    drawSensorWithBar("CO2 (ppm)", readings.co2ppm, 2000);
    drawSensorWithBar("VOC Index", readings.vocIndex, 500);

    y += dy / 2;

    // Warnings
    uint16_t warningStartY = y;
    int warningCount = 0;
    uint16_t iconX = x + 12 * dx;

    if (readings.temperatureF > 85) {
        drawWarningLine("Warning: High Temp!", drawHeatWarningIcon, y, x, dy, iconX);
        ++warningCount;
    }
    if (readings.humidityPercent > 70) {
        drawWarningLine("Warning: High Humidity!", drawHumidityIcon, y, x, dy, iconX);
        ++warningCount;
    }
    if (readings.aqi > 100) {
        drawWarningLine("Warning: Poor AQI!", drawAQIIcon, y, x, dy, iconX);
        ++warningCount;
    }

    if (warningCount > 0) {
        uint16_t boxX = x - 4;
        uint16_t boxY = warningStartY - 2;
        uint16_t boxWidth = 17.3 * dx;
        uint16_t boxHeight = warningCount * dy + 6;
        myScreen.setPenSolid(false);
        myScreen.dRectangle(boxX, boxY, boxWidth, boxHeight, myColours.black);
    }

    myScreen.flush();
}

void drawWarnings(const SensorReadings& readings, uint16_t& y, uint16_t x, uint16_t dx, uint16_t dy) {
    myScreen.selectFont(Font_Terminal8x12);

    int warningCount = 0;
    const uint16_t iconX = x + 12 * dx;
    const uint16_t warningStartY = y;

    if (hasHighTemp(readings)) {
        drawWarningLine("Warning: High Temp!", drawHeatWarningIcon, y, x, dy, iconX);
        ++warningCount;
    }
    if (hasHighHumidity(readings)) {
        drawWarningLine("Warning: High Humidity!", drawHumidityIcon, y, x, dy, iconX);
        ++warningCount;
    }
    if (hasPoorAQI(readings)) {
        drawWarningLine("Warning: Poor AQI!", drawAQIIcon, y, x, dy, iconX);
        ++warningCount;
    }

    if (warningCount > 0) {
        const uint16_t boxX = x - 4;
        const uint16_t boxY = warningStartY - 2;
        const uint16_t boxWidth = 17.3 * dx;
        const uint16_t boxHeight = warningCount * dy + 6;
        myScreen.setPenSolid(false);
        myScreen.dRectangle(boxX, boxY, boxWidth, boxHeight, myColours.black);
    }
}

void displaySensorPage(const SensorReadings& readings, int pageIndex, int totalBasePages, int totalPages) {
    myScreen.setOrientation(3);
    myScreen.clear();

    uint16_t xMax = myScreen.screenSizeX();
    uint16_t yMax = myScreen.screenSizeY();
    const uint16_t dx = xMax / gridCols;
    const uint16_t dy = yMax / gridRows;
    uint16_t x = dx;
    uint16_t y = dy;
    uint16_t barX = x + 7 * dx;

    myScreen.selectFont(Font_Terminal12x16);
    myScreen.gText(x, y, "Air Sensor Readings");
    y += 2 * dy;

    myScreen.selectFont(Font_Terminal8x12);

    // Draw based on page
    if (pageIndex == 0) {
        myScreen.gText(x, y, "Env Conditions");
        y += 2 * dy;
        drawSensorWithBar("Temp (F)", readings.temperatureF, 120, y, x, barX, dx, dy);
        drawSensorWithBar("Humidity (%)", readings.humidityPercent, 100, y, x, barX, dx, dy);
        drawSensorWithBar("AQI", readings.aqi, 200, y, x, barX, dx, dy);
    }
    else if (pageIndex == 1) {
        myScreen.gText(x, y, "Particulates");
        y += 2 * dy;
        drawSensorWithBar("PM1", readings.pm1, 200, y, x, barX, dx, dy);
        drawSensorWithBar("PM2.5", readings.pm25, 200, y, x, barX, dx, dy);
        drawSensorWithBar("PM10", readings.pm10, 200, y, x, barX, dx, dy);
    }
    else if (pageIndex == 2) {
        myScreen.gText(x, y, "Gas & VOCs");
        y += 2 * dy;
        drawSensorWithBar("CO2 (ppm)", readings.co2ppm, 2000, y, x, barX, dx, dy);
        drawSensorWithBar("VOC Index", readings.vocIndex, 500, y, x, barX, dx, dy);

    }
    else if (pageIndex == 3) {
        myScreen.gText(x, y, "Warnings");
        y += dy;
        y += dy / 2;
        drawWarnings(readings, y, x, dx, dy);
    }

    // Page indicator
    char buf[20];
    sprintf(buf, "Page %d/%d", pageIndex + 1, totalPages);
    myScreen.gText(xMax - 6 * dx, yMax - dy, buf);

    myScreen.flush();
}



void setup() {
    mySerial.begin(115200);
    delay(500);
    myScreen.begin();
    myScreen.regenerate();
    randomSeed(analogRead(A0));
}

void loop() {
    unsigned long now = millis();

    if (now - lastPageSwitch >= pageDuration) {
        lastPageSwitch = now;

        // Refresh sensor values once per full cycle
        if (currentPage == 0) {
            currentReadings = generateMockSensorData();
        }

        // Now that currentReadings is ready, calculate pages
        const int basePages = 3;
        const int warningCount = getWarningCount(currentReadings);
        const int totalPages = basePages + (warningCount > 2 ? 1 : (warningCount > 0 ? 1 : 0));

        // Display
        displaySensorPage(currentReadings, currentPage, basePages, totalPages);

        // Move to next page
        currentPage = (currentPage + 1) % totalPages;
    }
}



