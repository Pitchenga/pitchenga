#pragma once

#include <cmath>
#include <algorithm>

class Agc {
public:
    Agc(float alphaIn, float targetLevelIn, float maxGainIn);

    float process(float sample);
    void reset();

private:
    float alpha;
    float targetLevel;
    float maxGain;
    float runningEnvelope = 0.001f;
};
