#include "TwoPlayerSplitGame.h"

TwoPlayerSplitGame::TwoPlayerSplitGame(uint8_t zoneCount, uint8_t leftCount, uint32_t durationMs)
    : _zoneCount(zoneCount > MAX_ZONES ? MAX_ZONES : zoneCount),
      _leftCount(leftCount > _zoneCount ? _zoneCount : leftCount),
      _durationMs(durationMs) {}

void TwoPlayerSplitGame::begin(uint32_t nowMs) {
    _startMs = nowMs;
    _gameOver = false;
    _result = GameResult{};
    _result.zoneCount = _zoneCount;

    for (uint8_t i = 0; i < _zoneCount; ++i) _active[i] = true;
    _leftRemaining = _leftCount;
    _rightRemaining = _zoneCount - _leftCount;
}

void TwoPlayerSplitGame::onHit(uint8_t zone, uint16_t amplitude, uint32_t nowMs) {
    if (_gameOver || zone >= _zoneCount) return;

    if (!_active[zone]) {
        _result.extraHits++;
        return;
    }

    _active[zone] = false;
    _result.zones[zone].hit = true;
    _result.zones[zone].hitAtMs = nowMs - _startMs;
    _result.zones[zone].amplitude = amplitude;
    _result.totalHits++;

    if (isLeftZone(zone)) {
        --_leftRemaining;
    } else {
        --_rightRemaining;
    }
}

void TwoPlayerSplitGame::update(uint32_t nowMs) {
    if (_gameOver) return;
    _result.elapsedMs = nowMs - _startMs;

    bool leftDone = (_leftRemaining == 0);
    bool rightDone = (_rightRemaining == 0);

    if (leftDone || rightDone) {
        _gameOver = true;
        _result.completed = true;
        _result.winner = (leftDone && rightDone) ? 2 : (leftDone ? 0 : 1);
        return;
    }

    if (_result.elapsedMs >= _durationMs) {
        _gameOver = true;
        _result.completed = false;

        uint8_t leftCleared = _leftCount - _leftRemaining;
        uint8_t rightCleared = (_zoneCount - _leftCount) - _rightRemaining;
        if (leftCleared > rightCleared) _result.winner = 0;
        else if (rightCleared > leftCleared) _result.winner = 1;
        else _result.winner = 2; // draw
    }
}

void TwoPlayerSplitGame::render(ILedController& leds) const {
    for (uint8_t i = 0; i < _zoneCount; ++i) {
        if (_active[i]) {
            leds.setZoneColor(i, 255, 40, 0); // still live
        } else {
            leds.setZoneColor(i, 0, 255, 0); // cleared
        }
    }
    leds.show();
}

bool TwoPlayerSplitGame::isGameOver() const { return _gameOver; }
const GameResult& TwoPlayerSplitGame::result() const { return _result; }
