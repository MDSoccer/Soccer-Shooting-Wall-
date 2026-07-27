#pragma once
#include <cstdint>
#include "GameMode.h"

struct HitEvent {
    bool valid = false;
    uint8_t zone = 0;
    uint16_t amplitude = 0;
    uint32_t timeMs = 0;
};

// Turns raw per-scan analog readings from up to MAX_ZONES piezo channels into
// clean single-hit events, per the plan's false-hit mitigation rules: compare
// amplitudes of everything crossing threshold within a short window and keep
// only the strongest, then lock that zone out briefly so ringing/echo can't
// re-trigger it.
class HitArbiter {
public:
    HitArbiter(uint8_t zoneCount, uint16_t threshold, uint32_t windowMs, uint32_t lockoutMs);

    // Feed one full scan (one reading per zone) taken at time nowMs.
    // Returns a HitEvent with valid=true the instant a winner is finalized.
    HitEvent feed(const uint16_t* readings, uint32_t nowMs);

    void reset();

private:
    uint8_t _zoneCount;
    uint16_t _threshold;
    uint32_t _windowMs;
    uint32_t _lockoutMs;

    bool _windowOpen = false;
    uint32_t _windowStartMs = 0;
    uint8_t _bestZone = 0;
    uint16_t _bestAmplitude = 0;

    uint32_t _lockoutUntil[MAX_ZONES] = {0};
};
