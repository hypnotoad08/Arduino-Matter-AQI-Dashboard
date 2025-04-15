#include "PDLS_EXT4_Basic_Matter.h"
#include "hV_HAL_Peripherals.h"

Screen_EPD_EXT4_Fast myScreen(eScreen_EPD_290_KS_0F, boardArduinoNanoMatter);

void drawSensorBar(uint16_t x, uint16_t y, uint16_t width, uint16_t height, int value, int maxValue) {
    uint16_t barWidth = map(value, 0, maxValue, 0, width);

    // Optional: draw outer outline
    myScreen.setPenSolid(true);
    myScreen.dRectangle(x, y, barWidth, height, myColours.black);
}



void displayMockSensorData(int temp, int humidity, int aqi) {
    myScreen.setOrientation(3);
    myScreen.clear();

    // Grid dimensions
    uint16_t xMax = myScreen.screenSizeX();
    uint16_t yMax = myScreen.screenSizeY();
    uint16_t dx = xMax / 20; // horizontal unit
    uint16_t dy = yMax / 12; // vertical unit
    uint16_t x = dx;
    uint16_t y = dy;

    // Fonts
    myScreen.selectFont(Font_Terminal12x16);
    myScreen.gText(x, y, "Air Sensor Readings");
    y += 2 * dy;

    myScreen.selectFont(Font_Terminal8x12);

    // Temp
    myScreen.gText(x, y, formatString("Temp: %d F", temp));
    drawSensorBar(x + 7 * dx, y, 10 * dx, dy - 2, temp, 120);
    y += dy;

    // Humidity
    myScreen.gText(x, y, formatString("Humidity: %d%%", humidity));
    drawSensorBar(x + 7 * dx, y, 10 * dx, dy - 2, humidity, 100);
    y += dy;

    // AQI
    myScreen.gText(x, y, formatString("AQI: %d", aqi));
    drawSensorBar(x + 7 * dx, y, 10 * dx, dy - 2, aqi, 200);
    y += dy;

    // Warnings
    y += dy / 2;
    if (temp > 85) {
        myScreen.gText(x, y, "Warning: High Temp!");
        y += dy;
    }
    if (humidity > 70) {
        myScreen.gText(x, y, "Warning: High Humidity!");
        y += dy;
    }
    if (aqi > 100) {
        myScreen.gText(x, y, "Warning: Poor AQI!");
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
