#include "GameEngine.h"

GameEngine::GameEngine(SensorArray& sensors, LedController& leds, GridClearGame& game,
                        uint32_t countdownMs, uint32_t resultsMs)
    : _sensors(sensors), _leds(leds), _game(game), _countdownMs(countdownMs), _resultsMs(resultsMs) {}

void GameEngine::begin() { enterIdle(); }

void GameEngine::handleSerialCommands() {
    while (Serial.available()) {
        String line = Serial.readStringUntil('\n');
        line.trim();
        line.toLowerCase();
        if (line == "start" && _state == EngineState::IDLE) {
            enterCountdown(millis());
        } else if (line == "reset") {
            enterIdle();
        }
    }
}

void GameEngine::enterIdle() {
    _state = EngineState::IDLE;
    _stateEnteredMs = millis();
    _leds.clear();
    _leds.show();
    Serial.println(F("[idle] type 'start' to begin a round"));
}

void GameEngine::enterCountdown(uint32_t nowMs) {
    _state = EngineState::COUNTDOWN;
    _stateEnteredMs = nowMs;
    Serial.println(F("[countdown] get ready..."));
}

void GameEngine::renderCountdown(uint32_t nowMs) {
    bool on = ((nowMs / 250) % 2) == 0; // flash all live zones blue during countdown
    for (uint8_t z = 0; z < _game.result().zoneCount; ++z) {
        if (on) _leds.setZoneColor(z, 0, 0, 255);
        else _leds.setZoneColor(z, 0, 0, 0);
    }
    _leds.show();
}

void GameEngine::enterPlaying(uint32_t nowMs) {
    _state = EngineState::PLAYING;
    _stateEnteredMs = nowMs;
    _game.begin(nowMs);
    Serial.println(F("[playing] go!"));
}

void GameEngine::printResults() {
    const GameResult& r = _game.result();
    Serial.println(F("---- Round Results ----"));
    Serial.print(F("Completed: "));
    Serial.println(r.completed ? F("YES (cleared board)") : F("NO (time expired)"));
    Serial.print(F("Elapsed ms: "));
    Serial.println(r.elapsedMs);
    Serial.print(F("Hits: "));
    Serial.println(r.totalHits);
    Serial.print(F("Extra/wasted hits: "));
    Serial.println(r.extraHits);
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
                _game.onHit(hit.zone, hit.amplitude, nowMs);
            }
            _game.update(nowMs);
            _game.render(_leds);
            if (_game.isGameOver()) {
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
