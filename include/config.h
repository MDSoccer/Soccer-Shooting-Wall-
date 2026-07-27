#pragma once

// Hardware/tuning constants for the ESP32-S3 firmware. Values below are
// starting points based on the project plan's block diagram; verify/tune
// them against the real sensor, peak-hold, and mux circuit once built.
#include <cstdint>

// ---- Zone topology ----
// Number of target zones actually wired up right now. Phase 1 = 1,
// Phase 2 = 4, Phase 3 = 12. The CD74HC4067 mux supports up to 16.
constexpr uint8_t ZONE_COUNT = 4;

// ---- CD74HC4067 multiplexer control pins ----
// S0-S3 select one of 16 mux channels; SIG is the shared analog output fed
// into the ESP32-S3's ADC. Adjust these to match the actual wiring.
constexpr uint8_t MUX_S0_PIN = 5;
constexpr uint8_t MUX_S1_PIN = 6;
constexpr uint8_t MUX_S2_PIN = 9;
constexpr uint8_t MUX_S3_PIN = 10;
constexpr uint8_t MUX_SIG_PIN = 18; // ADC-capable GPIO wired to mux SIG
constexpr uint16_t MUX_SETTLE_US = 5; // settle time after switching address, before analogRead

// ---- WS2812B LED strip (behind the 74AHCT125 level shifter) ----
constexpr uint8_t LED_DATA_PIN = 11;
constexpr uint8_t LEDS_PER_ZONE = 1;
constexpr uint8_t LED_BRIGHTNESS = 80; // 0-255

// ---- Impact detection tuning ----
// HIT_THRESHOLD is in raw 12-bit ADC counts (0-4095) coming off the
// peak-hold circuit; it must be tuned once real sensors are wired in.
constexpr uint16_t HIT_THRESHOLD = 600;
constexpr uint32_t ARBITRATION_WINDOW_MS = 15; // plan calls for comparing amplitudes in a 10-20ms window
constexpr uint32_t HIT_LOCKOUT_MS = 150;       // plan calls for a 100-250ms per-zone lockout after a hit

// ---- Game timing defaults ----
constexpr uint32_t GRID_GAME_DURATION_MS = 30000;   // Grid Clear round length
constexpr uint32_t RANDOM_GAME_DURATION_MS = 30000; // Random Grid round length
constexpr uint8_t RANDOM_TARGET_COUNT = 2;          // how many of ZONE_COUNT zones are live targets each round
constexpr uint32_t SPLIT_GAME_DURATION_MS = 30000;  // Two-Player Split round length
// Zones [0, TWO_PLAYER_LEFT_COUNT) are the left player's; the rest are the
// right player's. Defaults to an even split of ZONE_COUNT.
constexpr uint8_t TWO_PLAYER_LEFT_COUNT = ZONE_COUNT / 2;

constexpr uint32_t COUNTDOWN_MS = 3000;
constexpr uint32_t RESULTS_DISPLAY_MS = 4000;
