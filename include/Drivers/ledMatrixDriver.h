#pragma once
#include <FastLED.h>
#include "Config.h"

// -----------------------------------------------------------------------
// LedMatrixDriver
//
// Pure hardware-facing wrapper around FastLED for a logical 32x8 EQ
// layout rendered onto a physical 8x32 WS2812B panel.
//
// This class knows NOTHING about "equalizers" or "loudness" - it only
// knows how to address pixels by (x, y) and push frames to the strip.
// All animation logic belongs in the Controller layer.
// -----------------------------------------------------------------------
class LedMatrixDriver {
public:
    LedMatrixDriver() {}

    void begin() {
        _controller = &FastLED.addLeds<WS2812B, Pins::LED_DATA, GRB>(_leds, MatrixGeo::NUM_LEDS);
        clear();
        show();
    }

    // Set a single pixel by grid coordinate.
    // x: 0..WIDTH-1 (bar index)
    // y: 0..HEIGHT-1 (row, 0 = top, HEIGHT-1 = bottom)
    void setPixel(uint8_t x, uint8_t y, const CRGB& color) {
        if (x >= MatrixGeo::WIDTH || y >= MatrixGeo::HEIGHT) return;
        _leds[xyToIndex(x, y)] = color;
    }

    void clear() {
        fill_solid(_leds, MatrixGeo::NUM_LEDS, CRGB::Black);
    }

    // Pushes this device's buffer out at its own brightness, independent
    // of any other FastLED-registered device (e.g. the WS2811 strip).
    void show() {
        if (_controller) _controller->showLeds(EqConfig::GLOBAL_BRIGHTNESS);
    }

private:
    CRGB _leds[MatrixGeo::NUM_LEDS];
    CLEDController* _controller = nullptr;

    // Rotate the logical 32x8 matrix clockwise onto the physical 8x32
    // panel, then apply serpentine wiring across the 32 physical rows.
    // That makes each EQ bar a vertical column while alternating rows
    // run in opposite directions.
    static uint16_t xyToIndex(uint8_t x, uint8_t y) {
        constexpr uint8_t PHYSICAL_WIDTH = MatrixGeo::HEIGHT;
        uint8_t physicalX = (MatrixGeo::HEIGHT - 1) - y;
        uint8_t physicalY = x;
        if (physicalY & 1) {
            physicalX = PHYSICAL_WIDTH - 1 - physicalX;
        }
        return static_cast<uint16_t>(physicalY) * PHYSICAL_WIDTH + physicalX;
    }
};
