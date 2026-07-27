#include "GridClearGame.h"

GridClearGame::GridClearGame(uint8_t zoneCount, uint32_t durationMs)
    : _zoneCount(zoneCount > MAX_ZONES ? MAX_ZONES : zoneCount), _durationMs(durationMs) {}

void GridClearGame::begin(uint32_t nowMs) {
    _startMs = nowMs;
    _gameOver = false;
    _result = GameResult{};
    _result.zoneCount = _zoneCount;
    for (uint8_t i = 0; i < _zoneCount; ++i) {
        _active[i] = true;
    }
}

void GridClearGame::onHit(uint8_t zone, uint16_t amplitude, uint32_t nowMs) {
    if (_gameOver || zone >= _zoneCount) return;

    if (!_active[zone]) {
        _result.extraHits++; // already cleared - a wasted hit
        return;
    }

    _active[zone] = false;
    _result.zones[zone].hit = true;
    _result.zones[zone].hitAtMs = nowMs - _startMs;
    _result.zones[zone].amplitude = amplitude;
    _result.totalHits++;
}

void GridClearGame::update(uint32_t nowMs) {
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
    } else if (_result.elapsedMs >= _durationMs) {
        _gameOver = true;
        _result.completed = false;
    }
}

void GridClearGame::render(ILedController& leds) const {
    for (uint8_t i = 0; i < _zoneCount; ++i) {
        if (_active[i]) {
            leds.setZoneColor(i, 255, 40, 0); // live target: amber/red
        } else {
            leds.setZoneColor(i, 0, 255, 0); // cleared: green
        }
    }
    leds.show();
}

bool GridClearGame::isGameOver() const { return _gameOver; }

const GameResult& GridClearGame::result() const { return _result; }
