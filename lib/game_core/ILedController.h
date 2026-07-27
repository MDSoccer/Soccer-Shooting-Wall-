#pragma once
#include <cstdint>

// Abstraction over the LED strip so game logic never touches Adafruit_NeoPixel
// directly. This is what makes GameMode implementations unit-testable on a
// desktop (see test/test_game_core) instead of only on real hardware.
class ILedController {
public:
    virtual void setZoneColor(uint8_t zone, uint8_t r, uint8_t g, uint8_t b) = 0;
    virtual void show() = 0;
    virtual void clear() = 0;
    virtual ~ILedController() = default;
};
