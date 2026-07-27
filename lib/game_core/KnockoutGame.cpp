#include "KnockoutGame.h"

KnockoutGame::KnockoutGame(uint8_t zoneCount, uint32_t durationMs)
    : _zoneCount(zoneCount > MAX_ZONES ? MAX_ZONES : zoneCount), _durationMs(durationMs) {}

void KnockoutGame::begin(uint32_t nowMs) {
    _startMs = nowMs;
    _gameOver = false;
    _result = GameResult{};
    _result.zoneCount = _zoneCount;
    _result.bestTimeMs = _bestClearTimeMs;
    for (uint8_t i = 0; i < _zoneCount; ++i) {
        _active[i] = true;
    }
}

void KnockoutGame::onHit(uint8_t zone, uint16_t amplitude, uint32_t nowMs) {
    if (_gameOver || zone >= _zoneCount) return;

    if (!_active[zone]) {
        _result.extraHits++; // already knocked out - a wasted hit
        return;
    }

    _active[zone] = false;
    _result.zones[zone].hit = true;
    _result.zones[zone].hitAtMs = nowMs - _startMs;
    _result.zones[zone].amplitude = amplitude;
    _result.totalHits++;
}

void KnockoutGame::update(uint32_t nowMs) {
    if (_gameOver) return;
    _result.elapsedMs = nowMs - _startMs;

    bool allCleared = true;
    for (uint8_t i = 0; i < _zoneCount; ++i) {
        if (_active[i]) {
            allCleared = false;
            break;
        }
    }

    if (allCleared) {
        _gameOver = true;
        _result.completed = true;
        if (_bestClearTimeMs == 0 || _result.elapsedMs < _bestClearTimeMs) {
            _bestClearTimeMs = _result.elapsedMs;
        }
        _result.bestTimeMs = _bestClearTimeMs;
    } else if (_result.elapsedMs >= _durationMs) {
        _gameOver = true;
        _result.completed = false;
        // Score for an incomplete round is the knockout count (result.totalHits) -
        // the best-clear-time record is only ever set by a full clear, so it's
        // left untouched here.
    }
}

void KnockoutGame::render(ILedController& leds) const {
    for (uint8_t i = 0; i < _zoneCount; ++i) {
        if (_active[i]) {
            leds.setZoneColor(i, 255, 40, 0); // live target: amber/red
        } else {
            leds.setZoneColor(i, 0, 255, 0); // knocked out: green
        }
    }
    leds.show();
}

bool KnockoutGame::isGameOver() const { return _gameOver; }

const GameResult& KnockoutGame::result() const { return _result; }
