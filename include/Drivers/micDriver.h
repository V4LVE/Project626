#pragma once
#include <Arduino.h>
#include "Config.h"

// -----------------------------------------------------------------------
// MicDriver
//
// Pure hardware-facing wrapper around the analog "Microphone Adjust
// Detection Module" (electret mic + comparator/gain pot board with an
// analog envelope-style OUT pin -- e.g. typical "Sound Detection Sensor"
// boards). This is NOT a digital I2S mic, so we can't FFT it usefully on
// an Uno R4; instead we sample it repeatedly over a short window and
// return the peak-to-peak swing, which is a solid proxy for loudness.
//
// This class knows nothing about envelopes/smoothing/EQ - it just gives
// you a raw peak-to-peak reading. Smoothing lives in LoudnessController.
// -----------------------------------------------------------------------
class MicDriver {
public:
    MicDriver() {}

    void begin() {
        pinMode(Pins::MIC_ANALOG, INPUT);
    }

    // Samples the analog pin as fast as possible for SAMPLE_WINDOW_MS
    // and returns the peak-to-peak (max - min) raw ADC reading.
    // Blocking for the duration of the window (default 50ms) - this is
    // intentional and keeps sampling deterministic and simple.
    uint16_t readPeakToPeak() const {
        uint16_t sampleMin = 4095; // Uno R4 ADC is 14-bit capable but
        uint16_t sampleMax = 0;    // analogRead() defaults to 10-bit (0-1023);
                                   // these bounds are generous either way.

        uint32_t startMs = millis();
        while (millis() - startMs < MicConfig::SAMPLE_WINDOW_MS) {
            uint16_t sample = analogRead(Pins::MIC_ANALOG);
            if (sample > sampleMax) sampleMax = sample;
            if (sample < sampleMin) sampleMin = sample;
        }

        if (sampleMax < sampleMin) return 0; // no samples taken (shouldn't happen)
        return sampleMax - sampleMin;
    }
};
