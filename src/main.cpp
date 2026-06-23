#include <Arduino.h>
#include "Config.h"
#include "../include/Drivers/micDriver.h"
#include "../src/Controllers/LoudnessController.h"

// Drivers (hardware-facing)
//LedMatrixDriver matrixDriver;
MicDriver micDriver;

// Controllers (logic-facing)
LoudnessController loudnessController;
//EqualizerController eqController;

uint32_t lastFrameMs = 0;

void setup() {
    Serial.begin(115200);

    //matrixDriver.begin();
    micDriver.begin();
    loudnessController.begin(&micDriver);
    //eqController.begin(&matrixDriver);
}

void loop() {
    // Sampling the mic is blocking for ~MicConfig::SAMPLE_WINDOW_MS (default
    // 50ms), which doubles as our frame pacing - no extra delay() needed.
    loudnessController.update();
    float loudness = loudnessController.getLoudness();

    //eqController.update(loudness);

    // Uncomment while tuning NOISE_FLOOR / LOUD_CEILING in Config.h:
    Serial.println(loudness);
}
