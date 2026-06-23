#pragma once
#include <Arduino.h>
#include "Config.h"
#include "../include/Drivers/micDriver.h"

// -----------------------------------------------------------------------
// LoudnessController
//
// Owns the "how loud is it right now, 0.0-1.0" value. Takes raw
// peak-to-peak ADC readings from MicDriver, normalizes them against the
// configured noise floor / loud ceiling, and applies asymmetric
// attack/decay smoothing so the value rises quickly on a loud hit but
// settles back down smoothly instead of jittering.
//
// This is the single source of truth for "loudness" that EqualizerController
// reads from. It has no idea LEDs exist.
// -----------------------------------------------------------------------
class LoudnessController {
public:
    LoudnessController() : _smoothedLoudness(0.0f) {}

    void begin(MicDriver* mic) {
        _mic = mic;
    }

    // Blocking call (~SAMPLE_WINDOW_MS) - call once per main loop iteration.
    void update() {
        uint16_t rawPP = _mic->readPeakToPeak();
        float normalized = normalize(rawPP);

        float rate = (normalized > _smoothedLoudness)
                         ? MicConfig::ENVELOPE_ATTACK
                         : MicConfig::ENVELOPE_DECAY;

        _smoothedLoudness += (normalized - _smoothedLoudness) * rate;
        _smoothedLoudness = constrain(_smoothedLoudness, 0.0f, 1.0f);
    }

    // 0.0 (silence) .. 1.0 (max configured loudness)
    float getLoudness() const {
        return _smoothedLoudness;
    }

private:
    MicDriver* _mic = nullptr;
    float _smoothedLoudness;

    static float normalize(uint16_t rawPP) {
        if (rawPP <= MicConfig::NOISE_FLOOR) return 0.0f;
        if (rawPP >= MicConfig::LOUD_CEILING) return 1.0f;

        return static_cast<float>(rawPP - MicConfig::NOISE_FLOOR) /
               static_cast<float>(MicConfig::LOUD_CEILING - MicConfig::NOISE_FLOOR);
    }
};
