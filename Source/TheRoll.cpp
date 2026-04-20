#include "TheRoll.h"
#include "Palette.h"
#include <algorithm>
#include <cmath>

TheRoll::TheRoll(PitchengaAudioProcessor& proc) : processor(proc) {}

void TheRoll::updateResults(const std::vector<SpectralPeak>& peaks) {
    // OUTDATED: MAnalyzer achieves its clean look by simply dropping the visual floor.
    // OUTDATED: Our raw data maps 0.0 to -90dB. A lot of low-level acoustic noise lives there.
    // OUTDATED: By setting the visual floor to 0.35 (approx -58dB), the noise falls entirely off the screen,
    // OUTDATED: leaving only the pure, distinct harmonic peaks.
    // OUTDATED: Stretch the remaining peaks back to the 0.0 - 1.0 range for drawing

    activePeaks = peaks;

    if (processor.uiSettings.showSteam) {
        pumpSteam();
    }

    repaint();
}

void TheRoll::resized() {
    const int width = getWidth();
    const int height = getHeight();

    if (width > 0 && height > 0) {
        // Initialize the rolling buffer whenever the window resizes
        steamImage = juce::Image(juce::Image::ARGB, width, height, true);
        steamScrollOffset = 0;
        paintFrame();
    }
}

juce::Font TheRoll::getLabelFont() {
    return {
        juce::FontOptions(15.0f)
        .withStyle("Bold")
        .withName(juce::Font::getDefaultMonospacedFontName())
    };
}

float TheRoll::getLabelAreaHeight() {
    return juce::GlyphArrangement::getStringWidth(getLabelFont(), "Ww8") + 4.0f;
}

float TheRoll::freqToMidi(float freq) {
    if (freq <= 0.0f) return 0.0f;
    return 69.0f + 12.0f * std::log2(freq / 440.0f);
}

float TheRoll::frequencyToX(float frequencyHz, float width) const {
    const float midi = freqToMidi(frequencyHz);
    return width * ((midi - minMidiNote) / (maxMidiNote - minMidiNote));
}

void TheRoll::paint(juce::Graphics& graphics) {
    if (!cachedFrame.isValid()) {
        paintFrame(); // Generates it if the engine wasn't ready during resized()
    }
    if (cachedFrame.isValid()) {
        graphics.drawImageAt(cachedFrame, 0, 0);
    }

    if (activePeaks.empty()) return;

    if (processor.uiSettings.showSteam) {
        paintSteam(graphics);
    }

    if (processor.uiSettings.showForrest) {
        paintPeaks(graphics);
    }
}

void TheRoll::paintFrame() {
    const int width = getWidth();
    const int height = getHeight();
    if (width <= 0 || height <= 0) return;

    // Create a transparent image (the 'true' flag clears it to zero alpha)
    cachedFrame = juce::Image(juce::Image::ARGB, width, height, true);
    juce::Graphics graphics(cachedFrame);
    paintFrame(graphics);
}

juce::String TheRoll::getNoteName(const int midiNote) {
    int chroma = midiNote % 12;
    if (chroma < 0) chroma += 12;
    const int octave = midiNote / 12 - 1;
    return Palette::chromaticScale[static_cast<size_t>(chroma)].toneName + juce::String(octave);
}

void TheRoll::paintLabel(
    juce::Graphics& graphics,
    const float labelHeight,
    const float maxTextWidth,
    const int midiNote,
    const float targetCenter,
    const float startY,
    const juce::Colour baseColor
) const {
    // OUTDATED: Not drawing a half label

    const juce::Colour labelColor = juce::Colours::black.interpolatedWith(baseColor, 0.6f);
    const juce::String name = getNoteName(midiNote);

    graphics.setColour(labelColor);
    graphics.saveState();
    graphics.addTransform(
        juce::AffineTransform::rotation(-juce::MathConstants<float>::halfPi, targetCenter, startY - 2.0f)
    );

    graphics.drawText(
        name,
        juce::Rectangle(targetCenter, startY - 2.0f - labelHeight / 2.0f, maxTextWidth, labelHeight),
        juce::Justification::centredLeft,
        false
    );
    graphics.restoreState();
}

void TheRoll::paintFrame(juce::Graphics& graphics) const {
    const auto height = static_cast<float>(getHeight());
    const float halfHeight = height * 0.5f;

    const juce::Font labelFont = getLabelFont();
    graphics.setFont(labelFont);
    const float labelHeight = labelFont.getHeight();
    const float maxTextWidth = juce::GlyphArrangement::getStringWidth(labelFont, "Ww8");

    const int startMidi = static_cast<int>(std::ceil(minMidiNote));
    const int endMidi = static_cast<int>(std::floor(maxMidiNote));

    for (int i = startMidi; i <= endMidi; ++i) {
        const int chroma = i % 12;

        // fixme: move to Tone
        // Identify standard "black" keys
        const bool isBlackKey = chroma == 1 || chroma == 3 || chroma == 6 || chroma == 8 || chroma == 10;

        // OUTDATED: Find the exact pitch bin index for this semitone
        // OUTDATED: Find the visual center of that specific pitch bin
        const float hz = 440.0f * std::pow(2.0f, (static_cast<float>(i) - 69.0f) / 12.0f);
        const float targetCenter = frequencyToX(hz, static_cast<float>(getWidth()));

        // Route the line to the top half (black keys) or bottom half (white keys)
        const float startY = getLabelAreaHeight();
        const float endY = isBlackKey ? halfHeight : height;

        const juce::Colour baseColor = Palette::chromaticScale[static_cast<size_t>(chroma)].color;
        const juce::Colour gridColor = juce::Colours::black.interpolatedWith(baseColor, 0.1f);
        graphics.setColour(gridColor);
        graphics.drawLine(targetCenter, startY, targetCenter, endY, 1.0f);

        paintLabel(graphics, labelHeight, maxTextWidth, i, targetCenter, startY, baseColor);
    }
}

void TheRoll::paintPeaks(juce::Graphics& graphics) const {
    const int width = getWidth();
    const int height = getHeight();

    // Configurable razor-sharp stems for the Forrest
    constexpr float stemWidthPixels = 5.0f;

    for (const auto& peak : activePeaks) {
        const float xPos = frequencyToX(peak.frequencyHz, static_cast<float>(width));

        if (xPos >= 0.0f && xPos <= static_cast<float>(width)) {
            const float normalizedMagnitude = std::min(1.0f, std::max(0.0f, peak.magnitude));
            const auto barHeight = normalizedMagnitude * static_cast<float>(height);

            float midi = freqToMidi(peak.frequencyHz);
            float continuousChroma = std::fmod(midi, 12.0f);
            if (continuousChroma < 0.0f) continuousChroma += 12.0f;

            const juce::Colour color = Palette::getContinuousColor(continuousChroma);
            graphics.setColour(color);

            graphics.fillRoundedRectangle(
                xPos - (stemWidthPixels * 0.5f),
                static_cast<float>(height) - barHeight,
                stemWidthPixels,
                barHeight,
                2.0f
            );
        }
    }
}

void TheRoll::pumpSteam() {
    if (activePeaks.empty() || !steamImage.isValid()) return;

    const int width = getWidth();
    const int height = getHeight();
    if (width <= 0 || height <= 0) return;

    const int speedPx = static_cast<int>(steamSpeedPxPerFrame);

    // Advance the scroll offset and wrap it like a treadmill
    steamScrollOffset = (steamScrollOffset + speedPx) % height;

    // Calculate where in the image memory the new row should be drawn
    const int drawY = (height - speedPx + steamScrollOffset) % height;

    // Native JUCE memory wipe: clears the specific row to completely transparent
    steamImage.clear(juce::Rectangle(0, drawY, width, speedPx), juce::Colours::transparentBlack);

    juce::Graphics graphics(steamImage);

    // Configurable razor-sharp stems for the Steam
    constexpr float stemWidthPixels = 4.0f;

    for (const auto& peak : activePeaks) {
        if (peak.magnitude > 0.05f) { // Prevents rendering absolute silence noise
            const float xPos = frequencyToX(peak.frequencyHz, static_cast<float>(width));

            if (xPos >= 0.0f && xPos <= static_cast<float>(width)) {
                float midi = freqToMidi(peak.frequencyHz);
                float continuousChroma = std::fmod(midi, 12.0f);
                if (continuousChroma < 0.0f) continuousChroma += 12.0f;

                const juce::Colour baseColor = Palette::getContinuousColor(continuousChroma);
                constexpr float undimmingGain = 1.6f;

                const float clampedMag = std::min(1.0f, peak.magnitude * undimmingGain);
                const juce::Colour color = juce::Colours::black.interpolatedWith(baseColor, clampedMag);

                graphics.setColour(color);
                graphics.fillRect(
                    xPos - (stemWidthPixels * 0.5f),
                    static_cast<float>(drawY),
                    stemWidthPixels,
                    static_cast<float>(speedPx)
                );
            }
        }
    }
}

void TheRoll::paintSteam(const juce::Graphics& graphics) const {
    if (!steamImage.isValid()) return;

    const int height = getHeight();

    // Draw the two halves of the ring buffer to create a flawless infinite upward scroll
    graphics.drawImageAt(steamImage, 0, -steamScrollOffset);
    graphics.drawImageAt(steamImage, 0, height - steamScrollOffset);
}