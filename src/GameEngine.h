#pragma once
#include <Arduino.h>
#include "SensorArray.h"
#include "LedController.h"
#include "GameMode.h"

enum class EngineState { IDLE, COUNTDOWN, PLAYING, RESULTS };

// Top-level state machine: idle -> countdown -> playing -> results -> idle.
// Owns the sensor/LED hardware and drives whichever GameMode is currently
// selected out of `modes`. Serial is used as the MVP control/telemetry
// surface ("start" / "reset" / "mode" commands, results printout) until the
// Wi-Fi dashboard exists.
class GameEngine {
public:
    // `modes` must stay valid for the lifetime of the GameEngine (it just
    // stores the pointer array, it doesn't own the GameMode instances).
    GameEngine(SensorArray& sensors, LedController& leds, GameMode* const* modes, uint8_t modeCount,
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
    void printModeMenu();
    GameMode& currentMode() { return *_modes[_modeIndex]; }

    SensorArray& _sensors;
    LedController& _leds;
    GameMode* const* _modes;
    uint8_t _modeCount;
    uint8_t _modeIndex = 0;
    uint32_t _countdownMs;
    uint32_t _resultsMs;

    EngineState _state = EngineState::IDLE;
    uint32_t _stateEnteredMs = 0;
};
