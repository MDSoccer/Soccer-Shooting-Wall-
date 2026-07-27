#pragma once
#include <cstdint>

// Tiny xorshift32 PRNG. Not cryptographic - just deterministic and
// hardware-independent so RandomGridGame's target selection is unit
// testable, and so it can be re-seeded from real entropy (millis(), an
// analog noise pin, etc.) on the actual firmware.
class SimpleRng {
public:
    explicit SimpleRng(uint32_t seed) : _state(seed ? seed : 0xA5A5A5A5u) {}

    void reseed(uint32_t seed) { _state = seed ? seed : 0xA5A5A5A5u; }

    uint32_t next() {
        _state ^= _state << 13;
        _state ^= _state >> 17;
        _state ^= _state << 5;
        return _state;
    }

private:
    uint32_t _state;
};
