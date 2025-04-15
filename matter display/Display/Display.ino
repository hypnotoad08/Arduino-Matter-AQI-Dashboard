
#include "PDLS_EXT4_Basic_Matter.h"
#include "hV_HAL_Peripherals.h"

Screen_EPD_EXT4_Fast myScreen(eScreen_EPD_290_KS_0F, boardArduinoNanoMatter);

const uint16_t iconSize = 12;
const uint16_t warningIconOffsetY = 2;  // Visual centering adjustment

void drawHeatIcon(uint16_t x, uint16_t y, uint16_t size) {
    myScreen.dLine(x, y, size, 0, myColours.black);
    myScreen.dLine(x + 2, y + 4, size, 0, myColours.black);
    myScreen.dLine(x + 4, y + 8, size, 0, myColours.black);
}

void drawHeatWaveIcon(uint16_t x, uint16_t y, uint16_t size)
{
    y += 2;  // Adjust icon Y slightly downward for visual center
    myScreen.setPenSolid(true);
    for (int i = 0; i < 3; i++) {
        myScreen.dLine(x, y + i * 4, size, 0, myColours.black);
    }
}
void drawHumidityIcon(uint16_t x, uint16_t y, uint16_t size) {
    y += 2;  // Adjust icon Y slightly downward for visual center
    myScreen.circle(x + size / 2, y + size / 2, size / 4, myColours.black);
    myScreen.point(x + size / 2, y + size / 2 + 2, myColours.black);
}

void drawAQIIcon(uint16_t x, uint16_t y, uint16_t size) {
    y += 2;  // Adjust icon Y slightly downward for visual center
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

void displayMockSensorData(int temp, int humidity, int aqi) {
    myScreen.setOrientation(3);
    myScreen.clear();

    uint16_t xMax = myScreen.screenSizeX();
    uint16_t yMax = myScreen.screenSizeY();
    uint16_t dx = xMax / 20;
    uint16_t dy = yMax / 12;
    uint16_t x = dx;
    uint16_t y = dy;

    myScreen.selectFont(Font_Terminal12x16);
    myScreen.gText(x, y, "Air Sensor Readings");
    y += 2 * dy;

    myScreen.selectFont(Font_Terminal8x12);

    myScreen.gText(x, y, formatString("Temp: %d F", temp));
    drawSensorBar(x + 7 * dx, y, 10 * dx, dy - 2, temp, 120);
    y += dy;

    myScreen.gText(x, y, formatString("Humidity: %d%%", humidity));
    drawSensorBar(x + 7 * dx, y, 10 * dx, dy - 2, humidity, 100);
    y += dy;

    myScreen.gText(x, y, formatString("AQI: %d", aqi));
    drawSensorBar(x + 7 * dx, y, 10 * dx, dy - 2, aqi, 200);
// After AQI rendering
y += dy / 2;

uint16_t warningStartY = y;
int warningCount = 0;

myScreen.setPenSolid(true);

if (temp > 85) {
    myScreen.gText(x, y, "Warning: High Temp!");
    drawHeatWaveIcon(iconX, y + iconYOffset, iconSize);
    y += dy;
    warningCount++;
}
if (humidity > 70) {
    myScreen.gText(x, y, "Warning: High Humidity!");
    drawHumidityIcon(iconX, y + iconYOffset, iconSize);
    y += dy;
    warningCount++;
}
if (aqi > 100) {
    myScreen.gText(x, y, "Warning: Poor AQI!");
    drawAQIIcon(iconX, y + iconYOffset, iconSize);
    y += dy;
    warningCount++;
}

// Draw dynamic bounding box if there were any warnings
if (warningCount > 0) {
    myScreen.setPenSolid(false);
    uint16_t boxHeight = warningCount * dy;
    myScreen.dRectangle(x - 4, warningStartY - 2, xMax - 2 * dx, boxHeight + 4, myColours.black);
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
