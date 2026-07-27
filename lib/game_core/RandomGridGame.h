#pragma once
#include "GameMode.h"
#include "SimpleRng.h"

// Same win/lose rules as KnockoutGame (hit every live target before time
// runs out), but only a random subset of zones are targets each round; the
// rest stay dark for the whole round. Harder variant since players must
// recognize which panels are actually live, not just react to everything.
class RandomGridGame : public GameMode {
public:
    RandomGridGame(uint8_t zoneCount, uint8_t targetCount, uint32_t durationMs, uint32_t seed);

    void begin(uint32_t nowMs) override;
    void onHit(uint8_t zone, uint16_t amplitude, uint32_t nowMs) override;
    void update(uint32_t nowMs) override;
    void render(ILedController& leds) const override;
    bool isGameOver() const override;
    const GameResult& result() const override;
    const char* name() const override { return "Random Grid"; }
    void reseed(uint32_t seed) override { _rng.reseed(seed); }

    // Exposed mainly for tests; true if `zone` was chosen as a live target
    // for the round currently in progress.
    bool isTarget(uint8_t zone) const;

private:
    void pickTargets();

    uint8_t _zoneCount;
    uint8_t _targetCount;
    uint32_t _durationMs;
    SimpleRng _rng;

    uint32_t _startMs = 0;
    bool _gameOver = false;
    bool _isTarget[MAX_ZONES] = {false}; // zones chosen as this round's targets
    bool _active[MAX_ZONES] = {false};   // targets not yet hit
    GameResult _result;
};
