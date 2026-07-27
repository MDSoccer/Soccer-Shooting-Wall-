#include "GameEngine.h"

GameEngine::GameEngine(SensorArray& sensors, LedController& leds, GameMode* const* modes, uint8_t modeCount,
                        uint32_t countdownMs, uint32_t resultsMs)
    : _sensors(sensors), _leds(leds), _modes(modes), _modeCount(modeCount),
      _countdownMs(countdownMs), _resultsMs(resultsMs) {}

void GameEngine::begin() { enterIdle(); }

void GameEngine::printModeMenu() {
    Serial.print(F("[idle] game: "));
    Serial.println(currentMode().name());
    Serial.println(F("  'start'    begin a round"));
    Serial.println(F("  'mode'     switch to the next game"));
    Serial.println(F("  'mode <n>' jump to game index n (0-based)"));
}

void GameEngine::handleSerialCommands() {
    while (Serial.available()) {
        String line = Serial.readStringUntil('\n');
        line.trim();
        line.toLowerCase();

        if (line == "start" && _state == EngineState::IDLE) {
            enterCountdown(millis());
        } else if (line == "reset") {
            enterIdle();
        } else if (_state == EngineState::IDLE && line.startsWith("mode")) {
            String arg = line.substring(4);
            arg.trim();
            if (arg.length() == 0) {
                _modeIndex = (_modeIndex + 1) % _modeCount;
            } else {
                int idx = arg.toInt();
                if (idx >= 0 && idx < _modeCount) _modeIndex = (uint8_t)idx;
            }
            printModeMenu();
        }
    }
}

void GameEngine::enterIdle() {
    _state = EngineState::IDLE;
    _stateEnteredMs = millis();
    _leds.clear();
    _leds.show();
    printModeMenu();
}

void GameEngine::enterCountdown(uint32_t nowMs) {
    _state = EngineState::COUNTDOWN;
    _stateEnteredMs = nowMs;
    Serial.println(F("[countdown] get ready..."));
}

void GameEngine::renderCountdown(uint32_t nowMs) {
    bool on = ((nowMs / 250) % 2) == 0; // flash all live zones blue during countdown
    for (uint8_t z = 0; z < currentMode().result().zoneCount; ++z) {
        if (on) _leds.setZoneColor(z, 0, 0, 255);
        else _leds.setZoneColor(z, 0, 0, 0);
    }
    _leds.show();
}

void GameEngine::enterPlaying(uint32_t nowMs) {
    _state = EngineState::PLAYING;
    _stateEnteredMs = nowMs;
    currentMode().reseed(nowMs); // fresh randomness each round, no-op for deterministic modes
    currentMode().begin(nowMs);
    Serial.println(F("[playing] go!"));
}

void GameEngine::printResults() {
    const GameResult& r = currentMode().result();
    Serial.print(F("---- "));
    Serial.print(currentMode().name());
    Serial.println(F(" Results ----"));
    Serial.print(F("Completed: "));
    Serial.println(r.completed ? F("YES") : F("NO (time expired)"));
    Serial.print(F("Elapsed ms: "));
    Serial.println(r.elapsedMs);
    Serial.print(F("Hits: "));
    Serial.println(r.totalHits);
    Serial.print(F("Extra/wasted hits: "));
    Serial.println(r.extraHits);
    if (r.winner == 0) Serial.println(F("Winner: LEFT player"));
    else if (r.winner == 1) Serial.println(F("Winner: RIGHT player"));
    else if (r.winner == 2) Serial.println(F("Result: DRAW"));
    for (uint8_t z = 0; z < r.zoneCount; ++z) {
        Serial.print(F("  zone "));
        Serial.print(z);
        Serial.print(F(": "));
        if (r.zones[z].hit) {
            Serial.print(F("hit at "));
            Serial.print(r.zones[z].hitAtMs);
            Serial.println(F(" ms"));
        } else {
            Serial.println(F("not hit"));
        }
    }
    Serial.println(F("------------------------"));
}

void GameEngine::enterResults(uint32_t nowMs) {
    _state = EngineState::RESULTS;
    _stateEnteredMs = nowMs;
    printResults();
}

void GameEngine::update(uint32_t nowMs) {
    handleSerialCommands();

    HitEvent hit = _sensors.poll();

    switch (_state) {
        case EngineState::IDLE:
            break;

        case EngineState::COUNTDOWN:
            renderCountdown(nowMs);
            if (nowMs - _stateEnteredMs >= _countdownMs) {
                enterPlaying(nowMs);
            }
            break;

        case EngineState::PLAYING:
            if (hit.valid) {
                currentMode().onHit(hit.zone, hit.amplitude, nowMs);
            }
            currentMode().update(nowMs);
            currentMode().render(_leds);
            if (currentMode().isGameOver()) {
                enterResults(nowMs);
            }
            break;

        case EngineState::RESULTS:
            if (nowMs - _stateEnteredMs >= _resultsMs) {
                enterIdle();
            }
            break;
    }
}
