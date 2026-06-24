#include <Arduino.h>
#include "Config.h"
#include "../include/Drivers/ledMatrixDriver.h"
#include "../include/Drivers/ledStripDriver.h"
#include "../include/Drivers/micDriver.h"
#include "../src/Controllers/loudnessController.h"
#include "../src/Controllers/eqController.h"
#include "../src/Controllers/pulseController.h"

// Drivers (hardware-facing)
LedMatrixDriver matrixDriver;
LedStripDriver stripDriver;
MicDriver micDriver;

// Controllers (logic-facing)
LoudnessController loudnessController; // mic -> 0..1 loudness
EqualizerController eqController;       // loudness-driven matrix EQ
PulseController pulseController;        // time-driven strip pulse (mic-independent)

void setup() {
    Serial.begin(115200);

    matrixDriver.begin();
    stripDriver.begin();
    micDriver.begin();

    loudnessController.begin(&micDriver);
    eqController.begin(&matrixDriver);
    pulseController.begin(&stripDriver);
}

void loop() {
    // Sampling the mic is blocking for ~MicConfig::SAMPLE_WINDOW_MS (default
    // 50ms), which doubles as our frame pacing - no extra delay() needed.
    loudnessController.update();
    float loudness = loudnessController.getLoudness();

    // matrixDriver.setPixel(0, 0, CRGB::Red); // push the matrix frame (EQ) to the panel

    // for (uint8_t i = 0; i < MatrixGeo::WIDTH; i++) {
    //     matrixDriver.setPixel(i, 7, CRGB::Red);
    //     matrixDriver.show();
    //     delay(1000); // brief delay to visualize the pixel-by-pixel update
    // }

    eqController.update(loudness); // loudness-driven matrix EQ
    pulseController.update(); // independent of loudness entirely

    // Uncomment while tuning NOISE_FLOOR / LOUD_CEILING in Config.h:
    Serial.println(loudness);
}
