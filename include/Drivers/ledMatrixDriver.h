#pragma once
#include <FastLED.h>
#include "Config.h"

// -----------------------------------------------------------------------
// LedMatrixDriver
//
// Pure hardware-facing wrapper around FastLED for an 8x32 row-major
// WS2812B panel (no zigzag/serpentine wiring).
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
    // x: 0..WIDTH-1 (column / bar index)
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

    // Row-major mapping: every row runs in the same direction (no
    // serpentine reversal on odd rows). Adjust here if your panel turns
    // out to actually be wired differently than expected.
    static uint16_t xyToIndex(uint8_t x, uint8_t y) {
        return static_cast<uint16_t>(y) * MatrixGeo::WIDTH + x;
    }
};
