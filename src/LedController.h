#pragma once
#include <Adafruit_NeoPixel.h>
#include "ILedController.h"

// Real WS2812B driver (behind the 74AHCT125 level shifter). Maps each
// logical zone to one or more physical pixels.
class LedController : public ILedController {
public:
    LedController(uint8_t zoneCount, uint8_t ledsPerZone, uint8_t dataPin, uint8_t brightness);

    void begin();
    void setZoneColor(uint8_t zone, uint8_t r, uint8_t g, uint8_t b) override;
    void show() override;
    void clear() override;

private:
    uint8_t _zoneCount;
    uint8_t _ledsPerZone;
    Adafruit_NeoPixel _strip;
};
