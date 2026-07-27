#include "RandomGridGame.h"

RandomGridGame::RandomGridGame(uint8_t zoneCount, uint8_t targetCount, uint32_t durationMs, uint32_t seed)
    : _zoneCount(zoneCount > MAX_ZONES ? MAX_ZONES : zoneCount),
      _targetCount(targetCount > _zoneCount ? _zoneCount : targetCount),
      _durationMs(durationMs),
      _rng(seed) {}

void RandomGridGame::pickTargets() {
    uint8_t indices[MAX_ZONES];
    for (uint8_t i = 0; i < _zoneCount; ++i) indices[i] = i;

    // Fisher-Yates shuffle, then take the first _targetCount as this round's targets.
    for (uint8_t i = _zoneCount; i > 1; --i) {
        uint8_t j = _rng.next() % i;
        uint8_t tmp = indices[i - 1];
        indices[i - 1] = indices[j];
        indices[j] = tmp;
    }

    for (uint8_t i = 0; i < _zoneCount; ++i) _isTarget[i] = false;
    for (uint8_t i = 0; i < _targetCount; ++i) _isTarget[indices[i]] = true;
}

void RandomGridGame::begin(uint32_t nowMs) {
    _startMs = nowMs;
    _gameOver = false;
    _result = GameResult{};
    _result.zoneCount = _zoneCount;

    pickTargets();
    for (uint8_t i = 0; i < _zoneCount; ++i) {
        _active[i] = _isTarget[i];
    }
}

void RandomGridGame::onHit(uint8_t zone, uint16_t amplitude, uint32_t nowMs) {
    if (_gameOver || zone >= _zoneCount) return;

    if (!_active[zone]) {
        _result.extraHits++; // not a target, or already cleared
        return;
    }

    _active[zone] = false;
    _result.zones[zone].hit = true;
    _result.zones[zone].hitAtMs = nowMs - _startMs;
    _result.zones[zone].amplitude = amplitude;
    _result.totalHits++;
}

void RandomGridGame::update(uint32_t nowMs) {
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

void RandomGridGame::render(ILedController& leds) const {
    for (uint8_t i = 0; i < _zoneCount; ++i) {
        if (_active[i]) {
            leds.setZoneColor(i, 255, 40, 0); // live target
        } else if (_isTarget[i]) {
            leds.setZoneColor(i, 0, 255, 0); // cleared target
        } else {
            leds.setZoneColor(i, 0, 0, 0); // never a target this round
        }
    }
    leds.show();
}

bool RandomGridGame::isGameOver() const { return _gameOver; }
const GameResult& RandomGridGame::result() const { return _result; }
bool RandomGridGame::isTarget(uint8_t zone) const { return zone < _zoneCount && _isTarget[zone]; }
