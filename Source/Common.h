#pragma once
#include <juce_gui_basics/juce_gui_basics.h>


class Common {
public:
    Common();

    static constexpr float labelFontSize = 12.0f;
    static constexpr auto labelFontStyle = "Bold";
    static juce::Font getLabelFont() {
        return {
            juce::FontOptions(Common::labelFontSize)
            .withStyle(Common::labelFontStyle)
            .withName(juce::Font::getDefaultMonospacedFontName())
        };
    }

    static float getLabelHeight() {
        return juce::GlyphArrangement::getStringWidth(Common::getLabelFont(), "Ww8");
    }

    static float getLabelWidth() {
        return getLabelFont().getHeight();
    }

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Common)
};
