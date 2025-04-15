#include "PDLS_EXT4_Basic_Matter.h"
#include "hV_HAL_Peripherals.h"

Screen_EPD_EXT4_Fast myScreen(eScreen_EPD_290_KS_0F, boardArduinoNanoMatter);
const uint16_t warningIconSize = 12;

void drawFramedBar(uint16_t x, uint16_t y, uint16_t width, uint16_t height, int value, int maxValue)
{
    value = constrain(value, 0, maxValue);
    uint16_t barWidth = map(value, 0, maxValue, 0, width);

    // Frame outline
    myScreen.setPenSolid(false);
    myScreen.dRectangle(x, y, width, height, myColours.black);

    // Filled bar
    myScreen.setPenSolid(true);
    myScreen.dRectangle(x, y, barWidth, height, myColours.black);
}

void drawWarningTriangle(uint16_t x, uint16_t y, uint16_t size)
{
    myScreen.setPenSolid(false);
    myScreen.triangle(
        x + size / 2, y,
        x, y + size,
        x + size, y + size,
        myColours.black);
}

void drawHeatWaveIcon(uint16_t x, uint16_t y)
{
    myScreen.setPenSolid(true);
    for (int i = 0; i < 3; i++) {
        myScreen.dLine(x, y + i * 4, 6, 0, myColours.black);
    }
}

void drawHumidityDroplet(uint16_t x, uint16_t y)
{
    myScreen.setPenSolid(false);
    myScreen.circle(x + 6, y + 6, 6, myColours.black);
    myScreen.dLine(x + 6, y, 0, 6, myColours.black);
}

void displayMockSensorData(int temp, int humidity, int aqi)
{
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
    uint16_t barX = x + 7 * dx;
    uint16_t barW = 10 * dx;
    uint16_t barH = dy - 2;

    // Temp
    myScreen.gText(x, y, formatString("Temp: %d F", temp));
    drawFramedBar(barX, y, barW, barH, temp, 120);
    y += dy;

    // Humidity
    myScreen.gText(x, y, formatString("Humidity: %d%%", humidity));
    drawFramedBar(barX, y, barW, barH, humidity, 100);
    y += dy;

    // AQI
    myScreen.gText(x, y, formatString("AQI: %d", aqi));
    drawFramedBar(barX, y, barW, barH, aqi, 200);
    y += dy;

    // Warnings
    y += dy / 2;
    uint16_t iconX = xMax - (warningIconSize + dx);
    uint16_t iconY = y;

    if (temp > 85) {
        myScreen.gText(x, iconY, "Warning: High Temp!");
        drawHeatWaveIcon(iconX, iconY);
        iconY += dy;
    }

    if (humidity > 70) {
        myScreen.gText(x, iconY, "Warning: High Humidity!");
        drawHumidityDroplet(iconX, iconY);
        iconY += dy;
    }

    if (aqi > 100) {
        myScreen.gText(x, iconY, "Warning: Poor AQI!");
        drawWarningTriangle(iconX, iconY, warningIconSize);
        iconY += dy;
    }

    myScreen.flush();
}

void setup()
{
    mySerial.begin(115200);
    delay(500);
    myScreen.begin();
    myScreen.regenerate();
    randomSeed(analogRead(A0));
}

void loop()
{
    int temp = random(65, 95);
    int humidity = random(40, 90);
    int aqi = random(20, 160);
    displayMockSensorData(temp, humidity, aqi);
    delay(10000);
}
