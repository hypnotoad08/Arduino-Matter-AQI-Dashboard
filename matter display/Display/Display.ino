
#include "PDLS_EXT4_Basic_Matter.h"
#include "hV_HAL_Peripherals.h"

Screen_EPD_EXT4_Fast myScreen(eScreen_EPD_290_KS_0F, boardArduinoNanoMatter);

const uint16_t iconSize = 12;
const uint16_t iconYOffset = 2; 
const uint16_t gridCols = 20;
const uint16_t gridRows = 12;


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



void drawWarningLine(const char* const message, void (*iconFunc)(uint16_t, uint16_t, uint16_t),
                     uint16_t& y, uint16_t dx, uint16_t dy, uint16_t iconX)

{
    myScreen.gText(dx, y, message);
    iconFunc(iconX, y + iconYOffset, iconSize);
    y += dy;
}


void displayMockSensorData(int temp, int humidity, int aqi) {
    myScreen.setOrientation(3);
    myScreen.clear();

    // Grid system
    static uint16_t xMax = myScreen.screenSizeX();
    static uint16_t yMax = myScreen.screenSizeY();
    const uint16_t dx = xMax / gridCols;
    const uint16_t dy = yMax / gridRows;
    uint16_t x = dx;
    uint16_t y = dy;
    uint16_t sensorBarX = x + 7 * dx;

    const int TEMP_THRESHOLD = 85;
    const int HUMIDITY_THRESHOLD = 70;
    const int AQI_THRESHOLD = 100;  


    // Fonts
    myScreen.selectFont(Font_Terminal12x16);
    myScreen.gText(x, y, "Air Sensor Readings");
    y += 2 * dy;

    myScreen.selectFont(Font_Terminal8x12);

    // Sensor Readings
drawSensorWithBar("Temp", temp, 120, y, x, sensorBarX, dx, dy);
drawSensorWithBar("Humidity", humidity, 100, y, x, sensorBarX, dx, dy);
drawSensorWithBar("AQI", aqi, 200, y, x, sensorBarX, dx, dy);

    y += dy/2;

    // Warning section setup
    uint16_t warningStartY = y;
    int warningCount = 0;
    uint16_t iconX = x + 12 * dx;

    if (temp > TEMP_THRESHOLD) {
        drawWarningLine("Warning: High Temp!", drawHeatWarningIcon, y, x, dy, iconX);
        ++warningCount;
    }
    if (humidity > HUMIDITY_THRESHOLD) {
        drawWarningLine("Warning: High Humidity!", drawHumidityIcon, y, x, dy, iconX);
        ++warningCount;
    }
    if (aqi > AQI_THRESHOLD) {
         drawWarningLine("Warning: Poor AQI!", drawAQIIcon, y, x, dy, iconX);
        ++warningCount;
    }

    // Draw dynamic box around warnings
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


void setup() {
    mySerial.begin(115200);
    delay(500);
    myScreen.begin();
    myScreen.regenerate();
    randomSeed(analogRead(A0));
}

void loop() {
    int temp = random(65, 95);
    int humidity = random(40, 90);
    int aqi = random(20, 160);
    displayMockSensorData(temp, humidity, aqi);
    delay(10000);
}
