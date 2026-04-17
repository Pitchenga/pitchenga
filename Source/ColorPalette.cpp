#include "ColorPalette.h"

const std::array<Pitch, 12> ColorPalette::chromaticScale = {{
    { Tone::Do,  0, 16.35f, juce::Colour::fromRGB (255, 0, 0), "Red", "C" },
    { Tone::Ra,  1, 17.32f, juce::Colour::fromRGB (255, 87, 87), "Pink", "Db" },
    { Tone::Re,  2, 18.35f, juce::Colour::fromRGB (255, 87, 0), "Orange", "D" },
    { Tone::Me,  3, 19.45f, juce::Colour::fromRGB (171, 127, 0), "Ochre", "Eb" },
    { Tone::Mi,  4, 20.60f, juce::Colour::fromRGB (255, 255, 0), "Yellow", "E" },
    { Tone::Fa,  5, 21.83f, juce::Colour::fromRGB (0, 255, 0), "Green", "F" },
    { Tone::Fi,  6, 23.12f, juce::Colour::fromRGB (0, 171, 127), "Teal", "F#" },
    { Tone::So,  7, 24.50f, juce::Colour::fromRGB (0, 255, 255), "Cyan", "G" },
    { Tone::Le,  8, 25.96f, juce::Colour::fromRGB (87, 87, 223), "Lilac", "Ab" },
    { Tone::La,  9, 27.50f, juce::Colour::fromRGB (0, 0, 255), "Blue", "A" },
    { Tone::Te, 10, 29.14f, juce::Colour::fromRGB (127, 0, 255), "Violet", "Bb" },
    { Tone::Ti, 11, 30.87f, juce::Colour::fromRGB (255, 0, 255), "Magenta", "B" }
}};