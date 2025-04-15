
#include <Matter.h>

// PDLS
#include "PDLS_EXT4_Basic_Matter.h"
#include "hV_HAL_Peripherals.h"

Screen_EPD_EXT4_Fast myScreen(eScreen_EPD_290_KS_0F, boardArduinoNanoMatter);

uint16_t x = 4;
uint16_t y = 4;

void setup() {
    mySerial.begin(115200);
    delay(500);

    myScreen.begin();
    myScreen.clear();
    myScreen.setOrientation(3);
    myScreen.selectFont(Font_Terminal12x16);

    drawHeader();
    displaySensorData(22.5, 55.2, 42);  // mock values
}

void loop() {
    delay(60000); // refresh once per minute
    myScreen.clear();
    drawHeader();

    // Generate new mock values
    float temperature = 20.0 + random(-50, 50) / 10.0; // ~15–25 °C
    float humidity = 45.0 + random(-100, 100) / 10.0;  // ~35–55%
    uint16_t aqi = random(10, 100);                   // AQI 10–100

    displaySensorData(temperature, humidity, aqi);
}

void drawHeader() {
    y = 4;
    myScreen.gText(x, y, "Air Sensor Readings");
    y += myScreen.characterSizeY() + 4;
}

void displaySensorData(float tempC, float humidity, uint16_t aqi) {
    myScreen.selectFont(Font_Terminal8x12);
    uint16_t dy = myScreen.characterSizeY();

    myScreen.gText(x, y, formatString("Temp:    %.1f C", tempC));    y += dy;
    myScreen.gText(x, y, formatString("Humidity: %.1f %%", humidity)); y += dy;
    myScreen.gText(x, y, formatString("AQI:      %d", aqi));         y += dy;

    myScreen.flush();
}
