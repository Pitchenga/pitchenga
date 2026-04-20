#include "Agc.h"

Agc::Agc(float alphaIn, float targetLevelIn, float maxGainIn)
    : alpha(alphaIn), targetLevel(targetLevelIn), maxGain(maxGainIn) {}

float Agc::process(float sample) {
    // 1. Update the running average
    runningEnvelope = (1.0f - alpha) * runningEnvelope + alpha * std::abs(sample);

    // 2. Calculate the required gain (+ 0.00001f prevents divide by zero)
    float dynamicGain = targetLevel / (runningEnvelope + 0.00001f);

    // 3. Clamp to prevent pure silence from exploding into noise
    dynamicGain = std::min(dynamicGain, maxGain);

    // 4. Apply gain and Soft-Clip
    return std::tanh(sample * dynamicGain);
}

void Agc::reset() {
    runningEnvelope = 0.001f;
}
