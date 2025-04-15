#include "PDLS_EXT4_Basic_Matter.h"
#include "hV_HAL_Peripherals.h"

Screen_EPD_EXT4_Fast myScreen(eScreen_EPD_290_KS_0F, boardArduinoNanoMatter);

const uint16_t warningIconSize = 12;

void drawWarningTriangleWithDot(uint16_t x, uint16_t y, uint16_t size) {
    uint16_t midX = x + size / 2;
    myScreen.setPenSolid(false);
    myScreen.triangle(midX, y, x, y + size, x + size, y + size, myColours.black);
    myScreen.point(midX, y + size * 2 / 3, myColours.black);
}

void drawWarningWaveIcon(uint16_t x, uint16_t y, uint16_t width) {
    myScreen.setPenSolid(true);
    uint16_t waveHeight = 2;
    for (uint16_t i = 0; i < 3; ++i) {
        myScreen.dLine(x, y + i * 4, width, waveHeight, myColours.black);
    }
}

void drawWarningArrowIcon(uint16_t x, uint16_t y, uint16_t size) {
    uint16_t cx = x + size / 2;
    myScreen.dLine(cx, y, 0, size, myColours.black);
    myScreen.line(cx, y + size, x, y + size - 4, myColours.black);
    myScreen.line(cx, y + size, x + size, y + size - 4, myColours.black);
}

void drawSensorBar(uint16_t x, uint16_t y, uint16_t width, uint16_t height, int value, int maxValue) {
    value = constrain(value, 0, maxValue);
    uint16_t barWidth = map(value, 0, maxValue, 0, width);

    myScreen.setPenSolid(true);
    myScreen.dRectangle(x, y, barWidth, height, myColours.black); // fill
    myScreen.setPenSolid(false);
    myScreen.rectangle(x, y, x + width, y + height, myColours.black); // frame
}

void displayMockSensorData(int temp, int humidity, int aqi) {
    myScreen.setOrientation(3);
    myScreen.clear();

    static uint16_t xMax = myScreen.screenSizeX();
    static uint16_t yMax = myScreen.screenSizeY();
    uint16_t dx = xMax / 20;
    uint16_t dy = yMax / 12;
    uint16_t x = dx;
    uint16_t y = dy;

    myScreen.selectFont(Font_Terminal12x16);
    myScreen.gText(x, y, "Air Sensor Readings");
    y += 2 * dy;

    myScreen.selectFont(Font_Terminal8x12);

    // Sensor text + bar layout
    uint16_t barX = x + 9 * dx;
    uint16_t barWidth = 9 * dx;
    uint16_t barHeight = dy - 2;

    myScreen.gText(x, y, formatString("Temp: %d F", temp));
    drawSensorBar(barX, y, barWidth, barHeight, temp, 120);
    y += dy;

    myScreen.gText(x, y, formatString("Humidity: %d%%", humidity));
    drawSensorBar(barX, y, barWidth, barHeight, humidity, 100);
    y += dy;

    myScreen.gText(x, y, formatString("AQI: %d", aqi));
    drawSensorBar(barX, y, barWidth, barHeight, aqi, 200);
    y += dy;

    // Warnings
    y += dy / 2;
    if (temp > 85) {
        myScreen.gText(x, y, "Warning: High Temp!");
        drawWarningArrowIcon(barX + barWidth + 2, y, 7);
        y += dy;
    }
    if (humidity > 70) {
        myScreen.gText(x, y, "Warning: High Humidity!");
        drawWarningWaveIcon(barX + barWidth + 2, y + 3, 8);
        y += dy;
    }
    if (aqi > 100) {
        myScreen.gText(x, y, "Warning: Poor AQI!");
        drawWarningTriangleWithDot(barX + barWidth + 2, y, warningIconSize);
        y += dy;
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
