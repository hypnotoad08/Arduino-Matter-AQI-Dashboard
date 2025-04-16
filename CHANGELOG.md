## [Unreleased]

### Added
- Support for EXT4 onboard temperature and humidity sensor (HDC2080)
  - Reads I2C sensor data via `hV_HAL_Wire_transfer`
  - Converts temperature to Fahrenheit
  - Converts humidity to percentage
  - Integrates live EXT4 readings into Page 1 display

### Fixed
- Blank screen on boot by explicitly setting screen orientation
