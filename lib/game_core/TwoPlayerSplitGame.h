#pragma once
#include "GameMode.h"

// Zones [0, leftCount) belong to the left player, [leftCount, zoneCount) to
// the right player. Each side races to clear their own zones independently
// (same rules as KnockoutGame, per side). The round ends the instant either
// side fully clears, or when time runs out - whichever comes first. Winner
// is whoever cleared first (or cleared more, if the clock ran out first);
// simultaneous/equal clears are a draw.
class TwoPlayerSplitGame : public GameMode {
public:
    TwoPlayerSplitGame(uint8_t zoneCount, uint8_t leftCount, uint32_t durationMs);

    void begin(uint32_t nowMs) override;
    void onHit(uint8_t zone, uint16_t amplitude, uint32_t nowMs) override;
    void update(uint32_t nowMs) override;
    void render(ILedController& leds) const override;
    bool isGameOver() const override;
    const GameResult& result() const override;
    const char* name() const override { return "Two-Player Split"; }

private:
    bool isLeftZone(uint8_t zone) const { return zone < _leftCount; }

    uint8_t _zoneCount;
    uint8_t _leftCount;
    uint32_t _durationMs;

    uint32_t _startMs = 0;
    bool _gameOver = false;
    bool _active[MAX_ZONES] = {false};
    uint8_t _leftRemaining = 0;
    uint8_t _rightRemaining = 0;
    GameResult _result;
};
