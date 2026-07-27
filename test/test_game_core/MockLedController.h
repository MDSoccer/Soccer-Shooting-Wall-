#pragma once
#include "ILedController.h"
#include <array>

struct MockLedController : public ILedController {
    struct Color { uint8_t r = 0, g = 0, b = 0; };
    std::array<Color, 16> colors;
    int showCount = 0;

    void setZoneColor(uint8_t zone, uint8_t r, uint8_t g, uint8_t b) override {
        if (zone < colors.size()) colors[zone] = {r, g, b};
    }
    void show() override { showCount++; }
    void clear() override { colors.fill(Color{}); }
};
