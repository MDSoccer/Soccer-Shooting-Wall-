#pragma once
#include <Arduino.h>
#include "HitArbiter.h"

// Drives the CD74HC4067 mux to scan every piezo channel through a single ADC
// pin, then feeds each full scan into a HitArbiter for debouncing/arbitration.
class SensorArray {
public:
    SensorArray(uint8_t zoneCount, uint8_t s0, uint8_t s1, uint8_t s2, uint8_t s3,
                uint8_t sigPin, uint16_t settleUs,
                uint16_t threshold, uint32_t windowMs, uint32_t lockoutMs);

    void begin();

    // Scans every zone once and feeds the arbiter. Call as often as possible from loop().
    HitEvent poll();

private:
    void selectChannel(uint8_t channel);

    uint8_t _zoneCount;
    uint8_t _s0, _s1, _s2, _s3, _sigPin;
    uint16_t _settleUs;
    uint16_t _readings[MAX_ZONES];
    HitArbiter _arbiter;
};
