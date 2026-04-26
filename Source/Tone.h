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

        const juce::Colour c1 = chromaticScale[static_cast<size_t>(index1)].color;
        const juce::Colour c2 = chromaticScale[static_cast<size_t>(index2)].color;

        float h1, s1, v1, h2, s2, v2;
        c1.getHSB(h1, s1, v1);
        c2.getHSB(h2, s2, v2);

        if (std::abs(h2 - h1) > 0.5f) {
            if (h2 > h1) h1 += 1.0f;
            else h2 += 1.0f;
        }

        float h = h1 + fraction * (h2 - h1);
        if (h >= 1.0f) h -= 1.0f;
        if (h < 0.0f) h += 1.0f;

        const float s = s1 + fraction * (s2 - s1);
        const float v = v1 + fraction * (v2 - v1);

        return juce::Colour::fromHSV(h, s, v, 1.0f);
    }
};
