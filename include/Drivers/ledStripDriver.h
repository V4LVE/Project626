#pragma once
#include <FastLED.h>
#include "Config.h"

// -----------------------------------------------------------------------
// LedStripDriver
//
// Pure hardware-facing wrapper around FastLED for a WS2811 addressable
// strip used by the running-light controller. Set NUM_LEDS for your
// actual strip in Config.h.
//
// Note: this strip is actually behaving like a GRB device on the bench,
// so the FastLED color order is set to GRB below. If that changes with a
// different strip, adjust the template parameter here.
//
// Like LedMatrixDriver, this class knows nothing about "loudness" - it
// only knows how to store and push pixel color data. All animation logic
// belongs in the Controller layer.
// -----------------------------------------------------------------------
class LedStripDriver {
public:
    LedStripDriver() {}

    void begin() {
        _controller = &FastLED.addLeds<WS2811, Pins::STRIP_DATA, GRB>(_leds, StripGeo::NUM_LEDS);
        clear();
        show();
    }

    void setPixel(uint16_t index, const CRGB& color) {
        if (index >= StripGeo::NUM_LEDS) return;
        _leds[index] = color;
    }

    // Sets every pixel on the strip to the same color.
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
