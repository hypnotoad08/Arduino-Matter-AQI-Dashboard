
#include "PDLS_EXT4_Basic_Matter.h"
#include "hV_HAL_Peripherals.h"
#include <time.h>

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

enum PageType {
    ENV_CONDITIONS = 0,
    PARTICULATES,
    GAS_AND_VOCS,
    WARNINGS
};

const uint16_t defaultIconSize = 12;
const uint16_t iconYOffset = 2; 
const uint16_t gridCols = 20;
const uint16_t gridRows = 12;

const unsigned long pageSwitchDurationMs = 8000; // 8 seconds per page
unsigned long lastPageSwitch = 0;
int currentPage = 0;
const int totalPages = 3;
SensorReadings currentReadings;
String currentTimestamp;


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

String generateMockTimestamp() {
    int mockHour = random(0, 24);
    int mockMinute = random(0, 60);
    int mockSecond = random(0, 60);
    int mockMonth = 4;
    int mockDay = 15;
    int mockYear = 2025;

    char buffer[22];
    snprintf(buffer, sizeof(buffer), "%02d/%02d %02d:%02d:%02d",
             mockMonth, mockDay, mockHour, mockMinute, mockSecond);
    return String(buffer);
}

const char* formatTimestamp(time_t t) {
    static char buf[20];
    struct tm* tm_info = localtime(&t);
    snprintf(buf, sizeof(buf), "%02d/%02d %02d:%02d:%02d",
             tm_info->tm_mon + 1, tm_info->tm_mday,
             tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
    return buf;
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
                       uint16_t x, uint16_t dx, uint16_t dy, uint16_t xMax)
{
    const uint16_t labelX = x;
    const uint16_t valueX = x + 6 * dx;
    const uint16_t barX = x + 12 * dx;
    const uint16_t barWidth = xMax - barX - dx;

    const char* unit = getUnitForLabel(label);
    myScreen.gText(labelX, y, label);
    myScreen.gText(valueX, y, formatString("%d%s", value, unit));
    drawSensorBar(barX, y, barWidth, dy - 4, value, maxValue);

    y += dy;
}

const char* getUnitForLabel(const char* label) {
    if (strcmp(label, "Temp") == 0) {
        return " F";
    } else if (strcmp(label, "Humidity") == 0) {
        return " %";
    } else if (strcmp(label, "CO2") == 0) {
        return " ppm";
    } else if (strcmp(label, "PM1") == 0 || strcmp(label, "PM2.5") == 0 || strcmp(label, "PM10") == 0) {
        return " ug/m3";
    } else {
        return "";
    }
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
    iconFunc(iconX, y + iconYOffset, defaultIconSize);
    y += dy;
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

void displaySensorPage(const SensorReadings& readings, PageType pageIndex, int totalBasePages, int totalPages) {
    myScreen.setOrientation(3);
    myScreen.clear();

    const uint16_t xMax = myScreen.screenSizeX();
    const uint16_t yMax = myScreen.screenSizeY();
    const uint16_t dx = xMax / gridCols;
    const uint16_t dy = yMax / gridRows;
    const uint16_t x = dx;
    uint16_t y = dy;

    myScreen.selectFont(Font_Terminal12x16);
    myScreen.gText(x, y, "Air Sensor Readings");
    y += 2 * dy;

    myScreen.selectFont(Font_Terminal8x12);

    if (pageIndex == ENV_CONDITIONS) {
        myScreen.gText(x, y, "Env Conditions");
        y += 2 * dy;
        drawSensorWithBar("Temp", readings.temperatureF, 120, y, x, dx, dy, xMax);
        drawSensorWithBar("Humidity", readings.humidityPercent, 100, y, x, dx, dy, xMax);
        drawSensorWithBar("AQI", readings.aqi, 200, y, x, dx, dy, xMax);
    }
    else if (pageIndex == PARTICULATES) {
        myScreen.gText(x, y, "Particulates");
        y += 2 * dy;
        drawSensorWithBar("PM1", readings.pm1, 200, y, x, dx, dy, xMax);
        drawSensorWithBar("PM2.5", readings.pm25, 200, y, x, dx, dy, xMax);
        drawSensorWithBar("PM10", readings.pm10, 200, y, x, dx, dy, xMax);
    }
    else if (pageIndex == GAS_AND_VOCS) {
        myScreen.gText(x, y, "Gas & VOCs");
        y += 2 * dy;
        drawSensorWithBar("CO2", readings.co2ppm, 2000, y, x, dx, dy, xMax);
        drawSensorWithBar("VOC Index", readings.vocIndex, 500, y, x, dx, dy, xMax);
    }
    else if (pageIndex == WARNINGS) {
        myScreen.gText(x, y, "Warnings");
        y += dy;
        y += dy / 2;
        drawWarnings(readings, y, x, dx, dy);
    }

    myScreen.gText(x, yMax - dy, "Last update:");
    myScreen.gText(x + 6 * dx, yMax - dy, currentTimestamp.c_str());

    // Page Indicator
    char buf[20];
    snprintf(buf, sizeof(buf), "Page %d/%d", pageIndex + 1, totalPages);
    myScreen.gText(xMax - 6 * dx, yMax - dy, buf);

    myScreen.flush();
}




void setup() {
    mySerial.begin(115200);
    delay(100); // Reduced delay to 100ms for faster initialization
    myScreen.begin();
    myScreen.regenerate();
    randomSeed(analogRead(A0) + millis());
}

void loop() {
    unsigned long currentMillis = millis(); // Store the value of millis() in a local variable

    if (currentMillis - lastPageSwitch >= pageSwitchDurationMs) {
        lastPageSwitch = currentMillis;

        // Refresh sensor values once per full cycle
        if (currentPage == 0) {
            currentReadings = generateMockSensorData();
             currentTimestamp = generateMockTimestamp();
        }

        // Now that currentReadings is ready, calculate pages
        const int basePages = 3;
        const int warningsPerPage = 3; // Define how many warnings fit on one page
        const int warningCount = getWarningCount(currentReadings);
        const int warningPages = (warningCount + warningsPerPage - 1) / warningsPerPage; // Calculate required warning pages
        const int totalPages = basePages + warningPages;

        // Display the current page
        displaySensorPage(currentReadings, static_cast<PageType>(currentPage), basePages, totalPages);
        // Move to next page
        currentPage = (currentPage + 1) % totalPages;
    }
}