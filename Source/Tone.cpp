#include "Tone.h"

const std::array<Pitch, 12> Tone::chromaticScale = {
    {
        {ToneName::Do, "Do", 0, 16.35f, juce::Colour::fromRGB(255, 0, 0), "C"},
        {ToneName::Ra, "Ra", 1, 17.32f, juce::Colour::fromRGB(255, 127, 127), "Db"},
        {ToneName::Re, "Re", 2, 18.35f, juce::Colour::fromRGB(255, 87, 0), "D"},
        {ToneName::Me, "Me", 3, 19.45f, juce::Colour::fromRGB(223, 171, 87), "Eb"},
        {ToneName::Mi, "Mi", 4, 20.60f, juce::Colour::fromRGB(255, 255, 0), "E"},
        {ToneName::Fa, "Fa", 5, 21.83f, juce::Colour::fromRGB(0, 255, 0), "F"},
        {ToneName::Fi, "Fi", 6, 23.12f, juce::Colour::fromRGB(87, 171, 127), "F#"},
        {ToneName::So, "So", 7, 24.50f, juce::Colour::fromRGB(0, 255, 255), "G"},
        {ToneName::Le, "Le", 8, 25.96f, juce::Colour::fromRGB(87, 127, 223), "Ab"},
        {ToneName::La, "La", 9, 27.50f, juce::Colour::fromRGB(127, 0, 255), "A"},
        {ToneName::Te, "Te", 10, 29.14f, juce::Colour::fromRGB(223, 127, 255), "Bb"},
        {ToneName::Ti, "Ti", 11, 30.87f, juce::Colour::fromRGB(255, 0, 255), "B"}
    }
};
