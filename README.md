# Matter Air Quality Display

An Arduino Nano Matter-powered e-paper display that shows air quality and environmental sensor data, including temperature, humidity, AQI, VOCs, CO₂, and particulate matter levels. Designed for use with the EXT4 e-paper dev kit from [Pervasive Displays](https://www.pervasivedisplays.com).

## Features

- ✅ Rotating e-paper pages (8s interval) with live sensor data
- ✅ Page titles and visual bar indicators
- ✅ Built-in warning alerts with icons (e.g., High Temp, Poor AQI)
- ✅ Timestamps with mock time syncing
- ✅ Support for EXT4's onboard HDC2080 sensor (Temp & Humidity)
- ✅ Modular code split into readable, logical files
- 🚧 Matter integration in progress

## Hardware

- **Arduino Nano Matter**
- **EPDK-Matter EXT4 e-paper board** (2.9" E Ink)
- **SEN66 air quality sensor** (planned)
- **Thread (Matter over Thread) + UART configuration** (planned)

## Code Structure

The code is modularized and organized under `src/` into logical subfolders.

```
Display/
├── Display.ino                  # Main sketch, handles boot/setup and page cycling
└── src/
    ├── Core/
    │   ├── SensorReadings.h     # Struct holding all air quality values
    │   ├── PageTypes.h          # Enum values for each screen page
    │   └── Ext4SensorReader.h   # Reads EXT4 HDC2080 sensor data via I2C
    └── UI/
        ├── DrawingHelpers.h     # Renders sensor bars, text, and layout
        ├── WarningHelpers.h     # Renders warning messages and icons
        ├── PageRenderer.h       # Handles page-specific display logic
        └── DisplayManager.h     # E-paper flush and refresh logic
```

## License

This project incorporates code adapted from the [PDLS_EXT4_Basic_Matter SDK](https://github.com/PervasiveDisplays/PDLS_EXT4_Basic_Matter), licensed under [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/).

Therefore, this project is also licensed under the **Creative Commons Attribution-ShareAlike 4.0 International** license.

© 2025 Evan Lee

## Acknowledgements

- Huge thanks to [Pervasive Displays](https://github.com/PervasiveDisplays) for the EXT4 SDK and example code
- Based on their `EXT4_Weather` and `Matter_RGB` examples
