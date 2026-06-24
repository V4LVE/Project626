#pragma once
#include <FastLED.h>
#include "Config.h"
#include "../include/Drivers/ledStripDriver.h"

// -----------------------------------------------------------------------
// CometController
//
// Drives the WS2811 strip as a Danish-themed red-and-white comet chase -
// entirely time-based. It has NO dependency on loudness/the mic at all;
// it just needs millis() to keep moving.
//
// The strip holds a deep Danish red base while several bright white
// comet heads race around with long red tails.
// -----------------------------------------------------------------------
class CometController {
public:
    CometController() {}

    void begin(LedStripDriver* strip) {
        _strip = strip;
    }

    void update() {
        if (_strip == nullptr || StripGeo::NUM_LEDS == 0) return;

        static uint32_t lastUpdateMs = 0;
        if (millis() - lastUpdateMs < StripConfig::COMET_STEP_MS) {
            return;
        }
        lastUpdateMs = millis();

        const CRGB danishRed(StripConfig::COMET_RED_VALUE, 0, 0);
        _strip->setAll(danishRed);

        uint16_t baseHead = (millis() / StripConfig::COMET_STEP_MS) % StripGeo::NUM_LEDS;
        uint16_t spacing = StripGeo::NUM_LEDS / StripConfig::COMET_COUNT;

        for (uint8_t comet = 0; comet < StripConfig::COMET_COUNT; comet++) {
            uint16_t head = (baseHead + static_cast<uint16_t>(comet) * spacing) % StripGeo::NUM_LEDS;

            for (uint8_t tail = 0; tail < StripConfig::COMET_TAIL_LENGTH; tail++) {
                uint16_t index = (head + StripGeo::NUM_LEDS - tail) % StripGeo::NUM_LEDS;

                uint8_t intensity;
                if (tail < StripConfig::COMET_HEAD_WIDTH) {
                    intensity = 255;
                } else {
                    uint8_t tailSpan = StripConfig::COMET_TAIL_LENGTH - StripConfig::COMET_HEAD_WIDTH;
                    if (tailSpan == 0) {
                        intensity = 80;
                    } else {
                        uint8_t tailPos = tail - StripConfig::COMET_HEAD_WIDTH;
                        uint8_t fade = 255 - static_cast<uint8_t>((static_cast<uint16_t>(tailPos) * 255U) / tailSpan);
                        intensity = qadd8(100, fade);
                    }
                }

                _strip->setPixel(index, blend(danishRed, CRGB::White, intensity));
            }
        }

        _strip->show();
    }

private:
    LedStripDriver* _strip = nullptr;
};
