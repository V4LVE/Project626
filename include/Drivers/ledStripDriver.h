#pragma once
#include <FastLED.h>
#include "Config.h"

// -----------------------------------------------------------------------
// LedStripDriver
//
// Pure hardware-facing wrapper around FastLED for a WS2811 addressable
// strip used as a simple ambient glow (not a per-bar layout like the
// matrix). Set NUM_LEDS for your actual strip in Config.h.
//
// Note: WS2811 strips are conventionally wired RGB (not GRB like
// WS2812B) - if colors look swapped on your strip, change the color
// order template parameter below.
//
// Like LedMatrixDriver, this class knows nothing about "loudness" - it
// only knows how to push a single color to every pixel. All animation
// logic belongs in the Controller layer.
// -----------------------------------------------------------------------
class LedStripDriver {
public:
    LedStripDriver() {}

    void begin() {
        _controller = &FastLED.addLeds<WS2811, Pins::STRIP_DATA, RGB>(_leds, StripGeo::NUM_LEDS);
        clear();
        show();
    }

    // Sets every pixel on the strip to the same color (ambient glow).
    void setAll(const CRGB& color) {
        fill_solid(_leds, StripGeo::NUM_LEDS, color);
    }

    void clear() {
        fill_solid(_leds, StripGeo::NUM_LEDS, CRGB::Black);
    }

    // Pushes this device's buffer out at its own brightness, independent
    // of the matrix (or any other FastLED-registered device).
    void show() {
        if (_controller) _controller->showLeds(StripConfig::GLOBAL_BRIGHTNESS);
    }

private:
    CRGB _leds[StripGeo::NUM_LEDS];
    CLEDController* _controller = nullptr;
};
