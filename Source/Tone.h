#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <array>
#include <cmath>

enum class ToneName { Do, Ra, Re, Me, Mi, Fa, Fi, So, Le, La, Te, Ti };

struct Pitch {
    ToneName tone;
    juce::String toneName;
    int index;
    float frequency;
    juce::Colour color;
    juce::String letterNotation;
};

class Tone {
public:
    // The single source of truth for the chromatic scale
    static const std::array<Pitch, 12> chromaticScale;

    // Extracts a continuous interpolated color directly from a float chroma [0.0 to 12.0)
    static juce::Colour getContinuousColor(const float chroma) {
        float wrapped = std::fmod(chroma, 12.0f);
        if (wrapped < 0.0f) wrapped += 12.0f;

        const int index1 = static_cast<int>(std::floor(wrapped)) % 12;
        const int index2 = (index1 + 1) % 12;
        const float fraction = wrapped - std::floor(wrapped);

        return chromaticScale[static_cast<size_t>(index1)].color
            .interpolatedWith(
                chromaticScale[static_cast<size_t>(index2)].color,
                fraction
            );
    }
};
