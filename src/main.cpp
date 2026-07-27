#include <Arduino.h>
#include "config.h"
#include "SensorArray.h"
#include "LedController.h"
#include "KnockoutGame.h"
#include "RandomGridGame.h"
#include "TwoPlayerSplitGame.h"
#include "GameEngine.h"

SensorArray sensors(ZONE_COUNT, MUX_S0_PIN, MUX_S1_PIN, MUX_S2_PIN, MUX_S3_PIN,
                     MUX_SIG_PIN, MUX_SETTLE_US,
                     HIT_THRESHOLD, ARBITRATION_WINDOW_MS, HIT_LOCKOUT_MS);
LedController leds(ZONE_COUNT, LEDS_PER_ZONE, LED_DATA_PIN, LED_BRIGHTNESS);

KnockoutGame knockoutGame(ZONE_COUNT, KNOCKOUT_DURATION_MS);
RandomGridGame randomGridGame(ZONE_COUNT, RANDOM_TARGET_COUNT, RANDOM_GAME_DURATION_MS, /*seed*/ 0x1234ABCD);
TwoPlayerSplitGame twoPlayerGame(ZONE_COUNT, TWO_PLAYER_LEFT_COUNT, SPLIT_GAME_DURATION_MS);

// Knockout is first/default so a plain "start" plays it out of the box.
GameMode* gameModes[] = {&knockoutGame, &randomGridGame, &twoPlayerGame};
constexpr uint8_t GAME_MODE_COUNT = sizeof(gameModes) / sizeof(gameModes[0]);

GameEngine engine(sensors, leds, gameModes, GAME_MODE_COUNT, COUNTDOWN_MS, RESULTS_DISPLAY_MS);

void setup() {
    Serial.begin(115200);
    delay(200);
    sensors.begin();
    leds.begin();
    engine.begin();
}

void loop() {
    engine.update(millis());
}
