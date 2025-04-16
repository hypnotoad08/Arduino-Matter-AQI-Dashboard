## [v0.1.0] - 2025-04-16
### Added
- Modular codebase: split into `SensorReadings`, `PageRenderer`, `DisplayManager`, `DrawingHelpers`, and more
- Page-based grid layout (ENV, Particulates, Gas, Warnings)
- Sensor bars with aligned text, clean spacing, and warning icon support
- "Last update" timestamp displayed on all pages
- Integrated readings from EXT4 board's onboard HDC2080 sensor:
  - Temperature (Screen)
  - Humidity (Screen)

### Fixed
- Initial page render issue resolved with `setOrientation(3)`
- EPD ghosting minimized with regenerate/flush logic inspired by Matter RGB example

### Notes
- **Mock Matter air sensor values** still used (not live data)
- **Display device only**: This unit shows data from another Matter sensor but does not publish values
- First pre-release milestone for screen layout and sensor readout
