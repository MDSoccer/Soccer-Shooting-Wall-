#pragma once
#include <cstdint>
#include "ILedController.h"

constexpr uint8_t MAX_ZONES = 16; // CD74HC4067 mux channel limit

struct ZoneStat {
    bool hit = false;
    uint32_t hitAtMs = 0; // time of hit relative to game start
    uint16_t amplitude = 0;
};

struct GameResult {
    uint8_t zoneCount = 0;
    ZoneStat zones[MAX_ZONES];
    uint16_t totalHits = 0;
    uint16_t extraHits = 0; // hits on zones that weren't (or were no longer) live targets
    uint32_t elapsedMs = 0;
    bool completed = false; // true = all targets cleared before time expired
};

// Interface every game/drill mode implements. GameEngine only talks to this,
// so new modes (random target pattern, two-player split-grid, reaction
// drills, etc.) plug in without touching sensor or LED code.
class GameMode {
public:
    virtual void begin(uint32_t nowMs) = 0;
    virtual void onHit(uint8_t zone, uint16_t amplitude, uint32_t nowMs) = 0;
    virtual void update(uint32_t nowMs) = 0;
    virtual void render(ILedController& leds) const = 0;
    virtual bool isGameOver() const = 0;
    virtual const GameResult& result() const = 0;
    virtual const char* name() const = 0;
    virtual ~GameMode() = default;
};
