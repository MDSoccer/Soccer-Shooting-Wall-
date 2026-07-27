#pragma once
#include "GameMode.h"

// MVP game mode: every configured zone lights up as a live target. Hitting a
// zone clears it (turns it off/green) and it stays cleared until the round
// resets. The round ends the instant every zone is cleared, or when the time
// limit runs out first — whichever comes first.
class GridClearGame : public GameMode {
public:
    GridClearGame(uint8_t zoneCount, uint32_t durationMs);

    void begin(uint32_t nowMs) override;
    void onHit(uint8_t zone, uint16_t amplitude, uint32_t nowMs) override;
    void update(uint32_t nowMs) override;
    void render(ILedController& leds) const override;
    bool isGameOver() const override;
    const GameResult& result() const override;
    const char* name() const override { return "Grid Clear"; }

private:
    uint8_t _zoneCount;
    uint32_t _durationMs;
    uint32_t _startMs = 0;
    bool _gameOver = false;
    bool _active[MAX_ZONES] = {false}; // true while a zone still needs to be hit
    GameResult _result;
};
