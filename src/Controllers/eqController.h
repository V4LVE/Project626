#pragma once
#include <FastLED.h>
#include "Config.h"
#include "../include/Drivers/ledMatrixDriver.h"

// -----------------------------------------------------------------------
// EqualizerController
//
// Owns the 32 EQ bars' state (current height, peak-hold cap) and decides,
// frame by frame, how they should move and what color they should be,
// driven entirely by a single loudness value (0.0-1.0) it's handed each
// frame. It draws into a LedMatrixDriver but never touches FastLED/pins
// directly - that separation is what makes this swappable/testable.
//
// "More extreme as loudness rises" is implemented as THREE compounding
// effects, all scaled by the same loudness value:
//   1. Target bar height scales with loudness.
//   2. Per-bar random jitter around that target grows with loudness,
//      so quiet = calm steady bars, loud = chaotic spiky bars.
//   3. Color shifts from a cold hue/low saturation/dim value at low
//      loudness to a hot hue/full saturation/full brightness at high
//      loudness - so the whole panel gets visually "hotter" when loud.
//
// The companion WS2811 strip is handled separately by PulseController -
// it's a fully independent, time-based animation with no loudness
// dependency, so it doesn't belong in this class.
// -----------------------------------------------------------------------
class EqualizerController {
public:
    EqualizerController() {
        for (uint8_t i = 0; i < MatrixGeo::WIDTH; i++) {
            _barHeight[i] = 0.0f;
            _peakHeight[i] = 0.0f;
        }
    }

    void begin(LedMatrixDriver* matrix) {
        _matrix = matrix;
        randomSeed(analogRead(A1)); // floating/unused-ish pin for entropy
    }

    // loudness: 0.0 (silence) .. 1.0 (max loudness), from LoudnessController.
    void update(float loudness) {
        updateBarHeights(loudness);
        render(loudness);
        _matrix->show();
    }

private:
    LedMatrixDriver* _matrix = nullptr;
    float _barHeight[MatrixGeo::WIDTH];
    float _peakHeight[MatrixGeo::WIDTH];

    void updateBarHeights(float loudness) {
        // Jitter grows with loudness: near-silent input gives near-zero
        // jitter (steady low bars), loud input gives big per-bar swings
        // so bars look "extreme"/spiky rather than one uniform block.
        float jitterRange = loudness * EqConfig::JITTER_AMOUNT;

        for (uint8_t i = 0; i < MatrixGeo::WIDTH; i++) {
            float jitter = randomFloat(-jitterRange, jitterRange);
            float target = constrain(loudness + jitter, 0.0f, 1.0f);

            float rate = (target > _barHeight[i]) ? EqConfig::BAR_ATTACK : EqConfig::BAR_DECAY;
            _barHeight[i] += (target - _barHeight[i]) * rate;
            _barHeight[i] = constrain(_barHeight[i], 0.0f, 1.0f);

            if (EqConfig::PEAK_HOLD_ENABLED) {
                if (_barHeight[i] > _peakHeight[i]) {
                    _peakHeight[i] = _barHeight[i];
                } else {
                    _peakHeight[i] -= EqConfig::PEAK_FALL_SPEED;
                    if (_peakHeight[i] < _barHeight[i]) _peakHeight[i] = _barHeight[i];
                }
            }
        }
    }

    void render(float loudness) {
        _matrix->clear();

        for (uint8_t x = 0; x < MatrixGeo::WIDTH; x++) {
            uint8_t litRows = heightToRows(_barHeight[x]);
            CRGB barColor = colorForLoudness(loudness, _barHeight[x]);

            // y = 0 is top row; bars grow up from the bottom (y = HEIGHT-1).
            for (uint8_t row = 0; row < litRows; row++) {
                uint8_t y = (MatrixGeo::HEIGHT - 1) - row;
                _matrix->setPixel(x, y, barColor);
            }

            if (EqConfig::PEAK_HOLD_ENABLED) {
                uint8_t peakRow = heightToRows(_peakHeight[x]);
                if (peakRow > 0 && peakRow >= litRows) {
                    uint8_t y = (MatrixGeo::HEIGHT - 1) - (peakRow - 1);
                    // Peak cap rendered in white so it reads as a distinct marker.
                    _matrix->setPixel(x, y, CRGB(255, 255, 255));
                }
            }
        }
    }

    static uint8_t heightToRows(float height01) {
        return static_cast<uint8_t>(round(height01 * MatrixGeo::HEIGHT));
    }

    // Maps loudness + this bar's own height into a hue/sat/val that gets
    // visually "hotter" (redder, more saturated, brighter) as either the
    // overall loudness or this specific bar's height increases.
    static CRGB colorForLoudness(float loudness, float barHeight01) {
        // Blend loudness and this bar's height so tall bars in a quiet
        // moment still look a bit hot, and the whole panel still reacts
        // to overall loudness even on short bars.
        float intensityF = constrain((loudness * 0.6f) + (barHeight01 * 0.4f), 0.0f, 1.0f);
        uint8_t intensity8 = static_cast<uint8_t>(intensityF * 255.0f);

        uint8_t hue = lerp8by8(EqConfig::COLD_HUE, EqConfig::HOT_HUE, intensity8);
        uint8_t sat = lerp8by8(EqConfig::MIN_SATURATION, EqConfig::MAX_SATURATION, intensity8);
        uint8_t val = lerp8by8(EqConfig::MIN_VALUE, EqConfig::MAX_VALUE, intensity8);

        return CHSV(hue, sat, val);
    }

    static float randomFloat(float lo, float hi) {
        return lo + (hi - lo) * (static_cast<float>(random(0, 10001)) / 10000.0f);
    }
};
