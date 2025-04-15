#include "PDLS_EXT4_Basic_Matter.h"
#include "hV_HAL_Peripherals.h"

Screen_EPD_EXT4_Fast myScreen(eScreen_EPD_290_KS_0F, boardArduinoNanoMatter);

void drawWarningTriangleWithDot(uint16_t x, uint16_t y, uint16_t size) {
    myScreen.setPenSolid(false);
    uint16_t midX = x + size / 2; // Store the repeated calculation in a local variable
    myScreen.triangle(midX, y, x, y + size, x + size, y + size, myColours.black);
     myScreen.setPenSolid(true); // Set to draw a filled circl
    myScreen.circle(midX, y + size * 2 / 3, size / 10, myColours.black);  // Little dot as alert
}

void drawDropletIcon(uint16_t x, uint16_t y, uint16_t size) {
    uint16_t centerX = x + size / 2;
    uint16_t topY = y;
    uint16_t bottomY = y + size;

    myScreen.setPenSolid(false);

    // Pointed top (triangle part of the droplet)
    myScreen.triangle(
        centerX, topY,                 // top point
        x, bottomY - size / 3,        // bottom left
        x + size, bottomY - size / 3, // bottom right
        myColours.black
    );

    // Rounded bottom (circle part of the droplet)
    myScreen.circle(centerX, bottomY - size / 4, size / 4, myColours.black);
}

void drawHeatIcon(uint16_t x, uint16_t y, uint16_t width) {
    myScreen.dLine(x, y, width / 2, 0, myColours.black);
    myScreen.dLine(x, y + 3, width, 0, myColours.black);
    myScreen.dLine(x, y + 6, width / 2, 0, myColours.black);
}

void drawHumidityIcon(uint16_t x, uint16_t y, uint16_t size) {
    // Drop tip (triangle)
    myScreen.triangle(
        x + size / 2, y,
        x, y + size,
        x + size, y + size,
        myColours.black
    );
    // Drop bottom (circle overlap)
    myScreen.circle(x + size / 2, y + size, size / 4, myColours.black);
}


void drawThermometerIcon(uint16_t x, uint16_t y, uint16_t height) {
    uint16_t bulbRadius = height / 5;
    uint16_t stemHeight = height - bulbRadius * 2;
    uint16_t stemWidth = 2;

    uint16_t centerX = x + bulbRadius;
    uint16_t topY = y;
    uint16_t bulbY = y + stemHeight;

    myScreen.setPenSolid(true);

    // Draw stem as a vertical thin rectangle
    myScreen.dRectangle(centerX - stemWidth / 2, topY, stemWidth, stemHeight, myColours.black);

    // Draw bulb as a circle
    myScreen.circle(centerX, bulbY + bulbRadius, bulbRadius, myColours.black);
}

void drawSensorBar(uint16_t x, uint16_t y, uint16_t width, uint16_t height, int value, int maxValue) {
    value = constrain(value, 0, maxValue); // Ensure value is within range
    uint16_t barWidth = map(value, 0, maxValue, 0, width);

    myScreen.setPenSolid(true);
    myScreen.dRectangle(x, y, barWidth, height, myColours.black);
}



const uint16_t warningIconSize = 12; // Define a descriptive constant for the warning icon size

void displayMockSensorData(int temp, int humidity, int aqi) {
    myScreen.setOrientation(3);
    myScreen.clear();

    // Grid dimensions
    static uint16_t xMax = myScreen.screenSizeX();
    static uint16_t yMax = myScreen.screenSizeY();
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
        drawHeatIcon(x + 13 * dx, y + 2, 10);
        y += dy;
    }
    if (humidity > 70) {
        myScreen.gText(x, y, "Warning: High Humidity!");
        drawHumidityIcon(x + 13 * dx, y, warningIconSize);
        y += dy;
    }
    if (aqi > 100) {
        drawWarningTriangleWithDot(x + 12 * dx, y, warningIconSize);
        myScreen.gText(x, y, "Warning: Poor AQI!");
        y += dy;
    }

    myScreen.flush();
}

void setup() {

    mySerial.begin(115200);
    delay(500); // Allow time for the serial connection to stabilize

    // Initialize the screen and prepare it for use
    myScreen.begin();
    myScreen.regenerate(); // Refresh the screen to ensure a clean state

    // Seed the random number generator using an analog pin
    randomSeed(analogRead(A0));
}

void loop() {
    int temp = random(65, 95);
    int humidity = random(40, 90);
    int aqi = random(20, 160);

    displayMockSensorData(temp, humidity, aqi);
    delay(10000);
}
