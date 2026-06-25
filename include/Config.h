#pragma once
#include <Arduino.h>
#include <FastLED.h>

// =====================================================================
// HARDWARE PINS
// =====================================================================
namespace Pins {
    constexpr uint8_t LED_DATA   = 6;   // WS2812B matrix data line
    constexpr uint8_t STRIP_DATA = 7;   // WS2811 strip data line
    constexpr uint8_t STRIP_DATA2 = 8;  // WS2811 strip data line for second strip (if used)
    constexpr uint8_t MIC_ANALOG = A0;  // Microphone module analog OUT
}

// =====================================================================
// MATRIX GEOMETRY
// Logical EQ layout is 32 columns wide by 8 rows tall, then rotated
// clockwise in LedMatrixDriver onto the physical 8x32 panel.
// =====================================================================
namespace MatrixGeo {
    constexpr uint8_t WIDTH      = 32;          // number of bars
    constexpr uint8_t HEIGHT     = 8;           // bar height resolution
    constexpr uint16_t NUM_LEDS  = WIDTH * HEIGHT;
}

// =====================================================================
// STRIP GEOMETRY
// WS2811 addressable strip, run as a red-and-white comet chase theme
// alongside the matrix (no per-bar layout, and no mic/loudness
// dependency - see CometController).
// TUNE NUM_LEDS to your actual strip length once you've counted it.
// =====================================================================
namespace StripGeo {
    constexpr uint16_t NUM_LEDS = 300; // <-- set this to your real strip length
}

// =====================================================================
// MICROPHONE / LOUDNESS TUNING
// The "Microphone Adjust Detection Module" is a simple electret mic with
// an onboard comparator/gain pot and an analog envelope output (NOT raw
// audio, NOT I2S). We read it as a fluctuating analog level and track a
// running peak-to-peak envelope ourselves, rather than doing an FFT.
// =====================================================================
namespace MicConfig {
    constexpr uint16_t SAMPLE_WINDOW_MS = 50;   // window to find peak-peak per loudness sample
    constexpr float    ENVELOPE_ATTACK  = 0.50f; // 0..1, how fast loudness can rise (per update)
    constexpr float    ENVELOPE_DECAY   = 0.08f; // 0..1, how fast loudness falls (slower = smoother)

    // Raw peak-to-peak ADC counts that map to "silence" and "max loudness".
    // TUNE THESE to your room/mic gain pot: start the sketch, watch Serial,
    // shout/clap near the mic, and set NOISE_FLOOR just above idle noise
    // and LOUD_CEILING near the loudest peak-to-peak value you saw.
    constexpr uint16_t NOISE_FLOOR  = 20;
    constexpr uint16_t LOUD_CEILING = 500;
}

// =====================================================================
// EQUALIZER ANIMATION TUNING
// =====================================================================
namespace EqConfig {
    constexpr uint8_t  FRAME_MS         = 30;   // ~33 FPS animation update rate

    // Bar height response
    constexpr float    BAR_ATTACK       = 0.6f; // how fast bars jump up
    constexpr float    BAR_DECAY        = 0.15f;// how fast bars fall back down
    constexpr float    JITTER_AMOUNT    = 0.35f; // per-bar random variation (0..1), scales with loudness

    // Peak-hold "caps" like a classic hardware EQ (set to 0 to disable)
    constexpr bool      PEAK_HOLD_ENABLED = true;
    constexpr float     PEAK_FALL_SPEED   = 0.04f; // how fast the peak cap drifts down per frame

    // Color: bar color sweeps from COLD_HUE (calm) to HOT_HUE (loud).
    // FastLED hue wheel: 0=red, 96=green, 160=blue, 224=pink (0-255 wheel)
    constexpr uint8_t  COLD_HUE         = 160;  // blue
    constexpr uint8_t  HOT_HUE          = 0;    // red
    constexpr uint8_t  MIN_SATURATION   = 180;  // saturation floor (quiet)
    constexpr uint8_t  MAX_SATURATION   = 255;  // saturation ceiling (loud)
    constexpr uint8_t  MIN_VALUE        = 90;   // brightness floor (quiet) - keeps it visible at idle
    constexpr uint8_t  MAX_VALUE        = 255;  // brightness ceiling (loud)

    constexpr uint8_t  GLOBAL_BRIGHTNESS = 110; // per-controller brightness via showLeds() - keep modest, 256 LEDs draw real current
}

// =====================================================================
// STRIP (COMET) ANIMATION TUNING
// Red base with a hard-running white comet head inspired by Danish
// flag colors.
// Entirely time-based - NOT connected to the mic/loudness at all.
// =====================================================================
namespace StripConfig {
    constexpr uint16_t COMET_STEP_MS          = 10;   // smaller = faster chase
    constexpr uint8_t  COMET_COUNT            = 4;    // number of simultaneous comets
    constexpr uint8_t  COMET_TAIL_LENGTH      = 12;   // longer = more aggressive streak
    constexpr uint8_t  COMET_HEAD_WIDTH       = 3;    // bright white head thickness
    constexpr uint8_t  COMET_RED_VALUE        = 180;  // Danish flag red base intensity

    constexpr uint8_t  GLOBAL_BRIGHTNESS = 110; // per-controller brightness via showLeds() - independent of the matrix's
}
