#pragma once
#include "GameMode.h"

// The primary game: every configured zone lights up as a live target and
// must be knocked out (hit) within the round's time limit, full stop.
//
// Scoring:
//   - Clear every zone before time runs out -> your score is the clear time
//     (lower is better). The fastest clear time seen this session is
//     tracked as a running best/record.
//   - Time runs out first -> your score is the total number of zones you
//     knocked out.
class KnockoutGame : public GameMode {
public:
    KnockoutGame(uint8_t zoneCount, uint32_t durationMs);

    void begin(uint32_t nowMs) override;
    void onHit(uint8_t zone, uint16_t amplitude, uint32_t nowMs) override;
    void update(uint32_t nowMs) override;
    void render(ILedController& leds) const override;
    bool isGameOver() const override;
    const GameResult& result() const override;
    const char* name() const override { return "Knockout"; }

    // 0 = no successful full clear yet this session.
    uint32_t bestClearTimeMs() const { return _bestClearTimeMs; }

private:
    uint8_t _zoneCount;
    uint32_t _durationMs;
    uint32_t _startMs = 0;
    bool _gameOver = false;
    bool _active[MAX_ZONES] = {false}; // true while zone still needs to be knocked out
    uint32_t _bestClearTimeMs = 0;      // persists across rounds; 0 = no record yet
    GameResult _result;
};
