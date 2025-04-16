#include "PDLS_EXT4_Basic_Matter.h"
#include "hV_HAL_Peripherals.h"
#include "SensorReadings.h"
#include "DrawingHelpers.h"
#include "WarningHelpers.h"
#include "PageRenderer.h"
#include "PageTypes.h"

Screen_EPD_EXT4_Fast myScreen(eScreen_EPD_290_KS_0F, boardArduinoNanoMatter);

const unsigned long pageSwitchDurationMs = 8000;
unsigned long lastPageSwitch = 0;
int currentPage = 0;
SensorReadings currentReadings;
String currentTimestamp;

void setup() {
    mySerial.begin(115200);
    delay(100);
    myScreen.begin();
    myScreen.regenerate();
    randomSeed(analogRead(A0) + millis());
}

void loop() {
    unsigned long now = millis();

    if (now - lastPageSwitch >= pageSwitchDurationMs) {
        lastPageSwitch = now;

        if (currentPage == 0) {
            currentReadings = generateMockSensorData();
            currentTimestamp = generateMockTimestamp();
        }

        const int basePages = 3;
        const int warningPages = (getWarningCount(currentReadings) + 2) / 3;
        const int totalPages = basePages + warningPages;

        displaySensorPage(currentReadings, static_cast<PageType>(currentPage), currentTimestamp, basePages, totalPages);
        currentPage = (currentPage + 1) % totalPages;
    }
}
