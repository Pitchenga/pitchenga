#include "ColorPalette.h"

const std::array<Pitch, 12> ColorPalette::chromaticScale = {
    {
        {Tone::Do, "Do", 0, 16.35f, juce::Colour::fromRGB(255, 0, 0), "C"},
        {Tone::Ra, "Ra", 1, 17.32f, juce::Colour::fromRGB(255, 127, 127), "Db"},
        {Tone::Re, "Re", 2, 18.35f, juce::Colour::fromRGB(255, 87, 0), "D"},
        {Tone::Me, "Me", 3, 19.45f, juce::Colour::fromRGB(171, 171, 0), "Eb"},
        {Tone::Mi, "Mi", 4, 20.60f, juce::Colour::fromRGB(255, 255, 0), "E"},
        {Tone::Fa, "Fa", 5, 21.83f, juce::Colour::fromRGB(0, 255, 0), "F"},
        {Tone::Fi, "Fi", 6, 23.12f, juce::Colour::fromRGB(0, 171, 127), "F#"},
        {Tone::So, "So", 7, 24.50f, juce::Colour::fromRGB(0, 255, 255), "G"},
        {Tone::Le, "Le", 8, 25.96f, juce::Colour::fromRGB(87, 127, 223), "Ab"},
        {Tone::La, "La", 9, 27.50f, juce::Colour::fromRGB(127, 0, 255), "A"},
        {Tone::Te, "Te", 10, 29.14f, juce::Colour::fromRGB(223, 127, 255), "Bb"},
        {Tone::Ti, "Ti", 11, 30.87f, juce::Colour::fromRGB(255, 0, 255), "B"}
    }
};
