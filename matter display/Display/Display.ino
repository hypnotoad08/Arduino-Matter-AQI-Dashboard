#include "PDLS_EXT4_Basic_Matter.h"
#include "hV_HAL_Peripherals.h"
#include "SensorReadings.h"
#include "DrawingHelpers.h"
#include "WarningHelpers.h"
#include "PageRenderer.h"
#include "PageTypes.h"
#include "DisplayManager.h"


Screen_EPD_EXT4_Fast myScreen(eScreen_EPD_290_KS_0F, boardArduinoNanoMatter);

const unsigned long pageSwitchDurationMs = 8000;
unsigned long lastPageSwitch = 0;
int currentPage = 0;
SensorReadings currentReadings;
String currentTimestamp;



void setup() {
    mySerial.begin(115200);
    delay(100); // Short init delay
    myScreen.begin();
    myScreen.setPowerProfile(POWER_MODE_AUTO, POWER_SCOPE_GPIO_ONLY);
    myScreen.regenerate(); 
    myScreen.setOrientation(3);
    DisplayManager::reset();
    randomSeed(analogRead(A0) + millis());
    myScreen.
}

void loop() {
    unsigned long currentMillis = millis();

    if (currentMillis - lastPageSwitch >= pageSwitchDurationMs) {
        lastPageSwitch = currentMillis;

        // If we’re back to the first page, update readings
        if (currentPage == 0) {
            currentReadings = generateMockSensorData();
            currentTimestamp = generateMockTimestamp();
        }

        const int basePages = 3;
        const int warningsPerPage = 3;
        const int warningCount = getWarningCount(currentReadings);
        const int totalPages = basePages + (warningCount + warningsPerPage - 1) / warningsPerPage;

        displaySensorPage(currentReadings, static_cast<PageType>(currentPage), currentTimestamp, basePages, totalPages);

        currentPage = (currentPage + 1) % totalPages;
    }
}

