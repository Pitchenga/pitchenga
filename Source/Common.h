#pragma once
#include <juce_gui_basics/juce_gui_basics.h>


class Common {
public:
    Common() = delete;

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

    /** Fast float modulus specifically for chroma (12.0f). Optimized for positive musical ranges. */
    static inline float fast_fmod12(float x) {
        if (x >= 0.0f && x < 127.0f) {
            while (x >= 12.0f) x -= 12.0f;
            return x;
        }
        const float wrapped = std::fmod(x, 12.0f);
        return (wrapped < 0.0f) ? wrapped + 12.0f : wrapped;
    }

    /** Fast integer modulus for 12. Optimized for positive MIDI notes. */
    static inline int fast_mod12(int x) {
        if (x >= 0 && x < 127) {
            while (x >= 12) x -= 12;
            return x;
        }
        const int wrapped = x % 12;
        return (wrapped < 0) ? wrapped + 12 : wrapped;
    }

    /** Generic fast float modulus for positive numbers. */
    static inline float fast_fmod(float x, float y) {
        if (x >= 0.0f && x < y * 10.0f) {
            while (x >= y) x -= y;
            return x;
        }
        const float wrapped = std::fmod(x, y);
        return (wrapped < 0.0f) ? wrapped + y : wrapped;
    }

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Common)
};
