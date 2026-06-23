#pragma once
#include <FastLED.h>
#include "Config.h"
#include "../include/Drivers/ledStripDriver.h"

// -----------------------------------------------------------------------
// PulseController
//
// Drives the WS2811 strip as a slow, continuous "breathing" pulse with a
// slowly cycling color - entirely time-based. It has NO dependency on
// loudness/the mic at all; it just needs millis() to keep moving.
//
// Brightness comes from beatsin8() (a smooth sine wave driven by a BPM),
// and hue comes from a free-running counter that increments slowly every
// frame, wrapping around the 0-255 hue wheel.
// -----------------------------------------------------------------------
class PulseController {
public:
    PulseController() {}

    void begin(LedStripDriver* strip) {
        _strip = strip;
    }

    void update() {
        // Smooth brightness wave between MIN_VALUE and MAX_VALUE, looping
        // PULSE_BPM times per minute. beatsin8 tracks elapsed time itself
        // (via millis()), so nothing needs to be stored between frames.
        uint8_t brightness = beatsin8(StripConfig::PULSE_BPM, StripConfig::MIN_VALUE, StripConfig::MAX_VALUE);

        // Slowly drifting hue: advances a tiny amount each frame so the
        // color cycles gradually through the wheel rather than snapping.
        _hue += StripConfig::HUE_CYCLE_SPEED;

        _strip->setAll(CHSV(_hue, StripConfig::PULSE_SATURATION, brightness));
        _strip->show();
    }

private:
    LedStripDriver* _strip = nullptr;
    uint8_t _hue = 0;
};
