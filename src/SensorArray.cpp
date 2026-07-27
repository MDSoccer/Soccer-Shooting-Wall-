#include "SensorArray.h"

SensorArray::SensorArray(uint8_t zoneCount, uint8_t s0, uint8_t s1, uint8_t s2, uint8_t s3,
                          uint8_t sigPin, uint16_t settleUs,
                          uint16_t threshold, uint32_t windowMs, uint32_t lockoutMs)
    : _zoneCount(zoneCount > MAX_ZONES ? MAX_ZONES : zoneCount),
      _s0(s0), _s1(s1), _s2(s2), _s3(s3), _sigPin(sigPin), _settleUs(settleUs),
      _arbiter(zoneCount, threshold, windowMs, lockoutMs) {}

void SensorArray::begin() {
    pinMode(_s0, OUTPUT);
    pinMode(_s1, OUTPUT);
    pinMode(_s2, OUTPUT);
    pinMode(_s3, OUTPUT);
    analogReadResolution(12); // 0-4095, matches HIT_THRESHOLD scale in config.h
}

void SensorArray::selectChannel(uint8_t channel) {
    digitalWrite(_s0, channel & 0x01);
    digitalWrite(_s1, (channel >> 1) & 0x01);
    digitalWrite(_s2, (channel >> 2) & 0x01);
    digitalWrite(_s3, (channel >> 3) & 0x01);
}

HitEvent SensorArray::poll() {
    for (uint8_t z = 0; z < _zoneCount; ++z) {
        selectChannel(z);
        if (_settleUs > 0) delayMicroseconds(_settleUs);
        _readings[z] = analogRead(_sigPin);
    }
    return _arbiter.feed(_readings, millis());
}
