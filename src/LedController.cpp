#include "LedController.h"

LedController::LedController(uint8_t zoneCount, uint8_t ledsPerZone, uint8_t dataPin, uint8_t brightness)
    : _zoneCount(zoneCount),
      _ledsPerZone(ledsPerZone),
      _strip(zoneCount * ledsPerZone, dataPin, NEO_GRB + NEO_KHZ800) {
    _strip.setBrightness(brightness);
}

void LedController::begin() {
    _strip.begin();
    _strip.show();
}

void LedController::setZoneColor(uint8_t zone, uint8_t r, uint8_t g, uint8_t b) {
    if (zone >= _zoneCount) return;
    uint32_t color = _strip.Color(r, g, b);
    uint16_t first = zone * _ledsPerZone;
    for (uint8_t i = 0; i < _ledsPerZone; ++i) {
        _strip.setPixelColor(first + i, color);
    }
}

void LedController::show() { _strip.show(); }

void LedController::clear() { _strip.clear(); }
