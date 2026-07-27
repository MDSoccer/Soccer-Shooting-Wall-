#pragma once

// ---- Zone count -----------------------------------------------------------
// Phase 1 = 1, Phase 2 = 4, Phase 3 = 12. The scan/lockout logic in main.cpp
// is written against this constant so scaling up is a wiring + config change,
// not a rewrite.
#define NUM_ZONES 1
#define LEDS_PER_ZONE 1 // adjust once panel LED layout is decided

// ---- Pin map ----------------------------------------------------------
// Adafruit Feather ESP32-S3 pin aliases. Verify against the board's pinout
// diagram/silkscreen before wiring — swap these if your board revision
// differs.
#define PIN_MUX_S0 5
#define PIN_MUX_S1 6
#define PIN_MUX_S2 9
#define PIN_MUX_S3 10
#define PIN_MUX_SIG A0 // CD74HC4067 common (SIG) pin -> ESP32 ADC input
#define PIN_LED_DATA 11 // -> 74AHCT125 level shifter -> WS2812B data in

// ---- Hit detection tuning --------------------------------------------------
// Placeholder values — calibrate against real piezo output once wired up.
#define ADC_HIT_THRESHOLD 800 // 0-4095 (12-bit ADC), analogRead() counts
#define SCAN_WINDOW_MS 15 // amplitude-comparison window (10-20ms per plan)
#define LOCKOUT_MS 150 // post-hit lockout per zone (100-250ms per plan)
#define LED_FLASH_MS 400 // how long a hit lights up its zone

#define ADC_RESOLUTION_BITS 12

// Set to 1 to stream raw ADC counts per zone over Serial (e.g. for the
// Arduino Serial Plotter) instead of running hit detection. Use this to
// tap the panel and find a real ADC_HIT_THRESHOLD before flipping back to 0.
#define DEBUG_ADC_STREAM 0

