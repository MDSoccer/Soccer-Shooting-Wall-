#pragma once
#include <Arduino.h>
#include "SensorArray.h"
#include "LedController.h"
#include "GridClearGame.h"

enum class EngineState { IDLE, COUNTDOWN, PLAYING, RESULTS };

// Top-level state machine: idle -> countdown -> playing -> results -> idle.
// Owns the sensor/LED hardware and drives whatever GameMode is currently
// active. Serial is used as the MVP control/telemetry surface ("start" /
// "reset" commands, results printout) until the Wi-Fi dashboard exists.
class GameEngine {
public:
    GameEngine(SensorArray& sensors, LedController& leds, GridClearGame& game,
               uint32_t countdownMs, uint32_t resultsMs);

    void begin();
    void update(uint32_t nowMs);

private:
    void handleSerialCommands();
    void enterIdle();
    void enterCountdown(uint32_t nowMs);
    void enterPlaying(uint32_t nowMs);
    void enterResults(uint32_t nowMs);
    void renderCountdown(uint32_t nowMs);
    void printResults();

    SensorArray& _sensors;
    LedController& _leds;
    GridClearGame& _game;
    uint32_t _countdownMs;
    uint32_t _resultsMs;

    EngineState _state = EngineState::IDLE;
    uint32_t _stateEnteredMs = 0;
};
