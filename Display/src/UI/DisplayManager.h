// Portions of this file are adapted from PDLS_EXT4_Basic_Matter
// https://github.com/PervasiveDisplays/PDLS_EXT4_Basic_Matter
// Licensed under CC BY-SA 4.0: https://creativecommons.org/licenses/by-sa/4.0/


#pragma once
#include "PDLS_EXT4_Basic_Matter.h"

extern Screen_EPD_EXT4_Fast myScreen;

namespace DisplayManager {
const uint8_t kFastBeforeGlobal = 8;
static uint8_t countFlush = 0;

/// @brief Flushes the screen with tracking
/// Regenerates the display every `kFastBeforeGlobal` updates to avoid ghosting
void flush() {
  countFlush++;
  if (countFlush >= kFastBeforeGlobal) {
    myScreen.regenerate();
    countFlush = 0;
  }
  myScreen.flush();
}

/// @brief Resets the flush counter, usually on boot or hard reset
void reset() {
  countFlush = 0;
}
}
