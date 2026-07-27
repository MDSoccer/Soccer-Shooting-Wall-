#include "HitArbiter.h"

HitArbiter::HitArbiter(uint8_t zoneCount, uint16_t threshold, uint32_t windowMs, uint32_t lockoutMs)
    : _zoneCount(zoneCount > MAX_ZONES ? MAX_ZONES : zoneCount),
      _threshold(threshold),
      _windowMs(windowMs),
      _lockoutMs(lockoutMs) {}

void HitArbiter::reset() {
    _windowOpen = false;
    _bestZone = 0;
    _bestAmplitude = 0;
    for (uint8_t i = 0; i < MAX_ZONES; ++i) _lockoutUntil[i] = 0;
}

HitEvent HitArbiter::feed(const uint16_t* readings, uint32_t nowMs) {
    for (uint8_t z = 0; z < _zoneCount; ++z) {
        if (nowMs < _lockoutUntil[z]) continue; // still ringing out from a previous hit

        uint16_t amp = readings[z];
        if (amp < _threshold) continue;

        if (!_windowOpen) {
            _windowOpen = true;
            _windowStartMs = nowMs;
            _bestZone = z;
            _bestAmplitude = amp;
        } else if (amp > _bestAmplitude) {
            _bestZone = z;
            _bestAmplitude = amp;
        }
    }

    HitEvent event;
    if (_windowOpen && (nowMs - _windowStartMs) >= _windowMs) {
        event.valid = true;
        event.zone = _bestZone;
        event.amplitude = _bestAmplitude;
        event.timeMs = nowMs;

        _lockoutUntil[_bestZone] = nowMs + _lockoutMs;
        _windowOpen = false;
        _bestAmplitude = 0;
    }
    return event;
}
