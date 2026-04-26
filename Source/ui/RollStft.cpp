#include "RollStft.h"
#include "../Tone.h"
#include "../math/Math.h"
#include "../Common.h"
#include <algorithm>
#include <cmath>

RollStft::RollStft(PitchengaAudioProcessor& proc) : processor(proc) {}

void RollStft::updateResults(const std::vector<SpectralPeak>& peaks) {
    if (!processor.settings.isUseRollStft
        || processor.settings.isFreezeRoll
        || !isVisible()
    ) {
        return;
    }

    activePeaks = peaks;
    pumpSteam();

    repaint();
}

void RollStft::resized() {
    const bool isHorizontal = processor.settings.isOrientationHorizontal;
    const int logicalWidth = isHorizontal ? getHeight() : getWidth();
    const int logicalHeight = isHorizontal ? getWidth() : getHeight();

    if (logicalWidth > 0 && logicalHeight > 0) {
        const int plotHeight = std::max(1, logicalHeight - static_cast<int>(getLabelAreaHeight()));
        // Initialize the rolling buffer whenever the window resizes
        steamImage = juce::Image(juce::Image::ARGB, logicalWidth, plotHeight, true);
        steamScrollOffset = 0;
        buildFrame();
    }
}

float RollStft::getLabelAreaHeight() const {
    if (!processor.settings.isShowRollLabels()) {
        return 0.0f;
    }
    return juce::GlyphArrangement::getStringWidth(Common::getLabelFont(), "Ww8") + 4.0f;
}

float RollStft::freqToMidi(float freq) {
    if (freq <= 0.0f) return 0.0f;
    return 69.0f + 12.0f * std::log2(freq / 440.0f);
}

float RollStft::frequencyToX(float frequencyHz, float width) {
    const float midi = freqToMidi(frequencyHz);
    return width * ((midi - minMidiNote) / (maxMidiNote - minMidiNote));
}

void RollStft::paint(juce::Graphics& graphics) {
    if (!cachedFrame.isValid()) {
        buildFrame();
    }

    const int physicalWidth = getWidth();
    const int physicalHeight = getHeight();
    const bool isHorizontal = processor.settings.isOrientationHorizontal;
    const int logicalWidth = isHorizontal ? physicalHeight : physicalWidth;
    const int logicalHeight = isHorizontal ? physicalWidth : physicalHeight;
    const float labelAreaHeight = getLabelAreaHeight();
    const int plotHeight = std::max(1, logicalHeight - static_cast<int>(labelAreaHeight));

    graphics.saveState();

    if (isHorizontal) {
        graphics.addTransform(juce::AffineTransform(0, 1, 0, -1, 0, physicalHeight));
    }

    if (cachedFrame.isValid()) {
        graphics.drawImageAt(cachedFrame, 0, 0);
    }

    if (processor.settings.isShowSteam) {
        paintSteam(graphics);
    }

    if (!activePeaks.empty() && processor.settings.isShowForrest) {
        paintForrest(graphics);
    }

    graphics.restoreState();
}

void RollStft::buildFrame() {
    const bool isHorizontal = processor.settings.isOrientationHorizontal;
    const int logicalWidth = isHorizontal ? getHeight() : getWidth();
    const int logicalHeight = isHorizontal ? getWidth() : getHeight();
    if (logicalWidth <= 0 || logicalHeight <= 0) return;

    // Create a transparent image (the 'true' flag clears it to zero alpha)
    cachedFrame = juce::Image(juce::Image::ARGB, logicalWidth, logicalHeight, true);
    juce::Graphics graphics(cachedFrame);
    paintFrame(graphics);
}

juce::String RollStft::getNoteName(const int midiNote) {
    int chroma = midiNote % 12;
    if (chroma < 0) chroma += 12;
    const int octave = midiNote / 12 - 1;
    return Tone::chromaticScale[static_cast<size_t>(chroma)].toneName + juce::String(octave);
}

void RollStft::paintLabel(
    juce::Graphics& graphics,
    const float labelHeight,
    const float maxTextWidth,
    const int midiNote,
    const float targetCenter,
    const float startY,
    const juce::Colour baseColor,
    const bool isHorizontal
) {
    //fixme: Un-hardcode
    if (midiNote == minMidiNote || midiNote == 108) {
        // Not drawing a half label
        return;
    }

    const juce::String name = getNoteName(midiNote);

    graphics.setColour(baseColor);
    graphics.saveState();

    if (isHorizontal) {
        const float rotX = targetCenter + labelHeight / 2.0f;
        const float rotY = startY - maxTextWidth;
        graphics.addTransform(
            juce::AffineTransform::rotation(juce::MathConstants<float>::halfPi, rotX, rotY)
        );
        graphics.drawText(
            name,
            juce::Rectangle<float>(rotX, rotY, maxTextWidth, labelHeight),
            juce::Justification::centredLeft,
            false
        );
    } else {
        graphics.addTransform(
            juce::AffineTransform::rotation(-juce::MathConstants<float>::halfPi, targetCenter, startY - 2.0f)
        );
        graphics.drawText(
            name,
            juce::Rectangle<float>(targetCenter, startY - 2.0f - labelHeight / 2.0f, maxTextWidth, labelHeight),
            juce::Justification::centredLeft,
            false
        );
    }

    graphics.restoreState();
}

void RollStft::paintFrame(juce::Graphics& graphics) const {
    const bool isHorizontal = processor.settings.isOrientationHorizontal;
    const int logicalWidth = isHorizontal ? getHeight() : getWidth();
    const int logicalHeight = isHorizontal ? getWidth() : getHeight();

    const auto totalHeight = static_cast<float>(logicalHeight);
    const float labelAreaHeight = getLabelAreaHeight();
    const float plotHeight = std::max(1.0f, totalHeight - labelAreaHeight);

    const juce::Font labelFont = Common::getLabelFont();
    graphics.setFont(labelFont);
    const float labelHeight = labelFont.getHeight();
    const float maxTextWidth = juce::GlyphArrangement::getStringWidth(labelFont, "Ww8");

    const int startMidi = static_cast<int>(std::ceil(minMidiNote));
    const int endMidi = static_cast<int>(std::floor(maxMidiNote));

    for (int midiNote = startMidi; midiNote <= endMidi; ++midiNote) {
        const int chroma = midiNote % 12;

        // fixme: move to ToneName
        // Identify standard "black" keys
        const bool isBlackKey = chroma == 1 || chroma == 3 || chroma == 6 || chroma == 8 || chroma == 10;

        const float hz = 440.0f * std::pow(2.0f, (static_cast<float>(midiNote) - 69.0f) / 12.0f);
        const float targetCenter = frequencyToX(hz, static_cast<float>(logicalWidth));

        const float startY = 0.0f;
        const float endY = plotHeight;

        const juce::Colour baseColor = Tone::chromaticScale[static_cast<size_t>(chroma)].color;
        const juce::Colour gridColor = juce::Colours::black.interpolatedWith(baseColor, 0.1f);
        graphics.setColour(gridColor);

        if (isBlackKey) {
            const float dashLengths[] = {4.0f, 4.0f};
            graphics.drawDashedLine(juce::Line<float>(targetCenter, startY, targetCenter, endY), dashLengths, 2, 1.0f);
        } else {
            graphics.drawLine(targetCenter, startY, targetCenter, endY, 1.0f);
        }

        if (processor.settings.isShowRollLabels()) {
            paintLabel(graphics, labelHeight, maxTextWidth, midiNote, targetCenter, totalHeight, baseColor, processor.settings.isOrientationHorizontal);
        }
    }
}

void RollStft::paintForrest(juce::Graphics& graphics) const {
    const bool isHorizontal = processor.settings.isOrientationHorizontal;
    const int logicalWidth = isHorizontal ? getHeight() : getWidth();
    const int logicalHeight = isHorizontal ? getWidth() : getHeight();

    const int width = logicalWidth;
    const float plotHeight = std::max(1.0f, static_cast<float>(logicalHeight) - getLabelAreaHeight());

    for (const auto& peak : activePeaks) {
        const float xPos = frequencyToX(peak.frequencyHz, static_cast<float>(width));

        if (xPos >= 0.0f && xPos <= static_cast<float>(width)) {
            // Configurable razor-sharp stems for the Forrest
            float stemWidthPixels = 5.0f;
            if (enableDynamicStemWidth) {
                const float nextX = frequencyToX(peak.frequencyHz + peak.bandwidthHz, static_cast<float>(width));
                // +1.0f forces deliberate sub-pixel overlap to completely kill rendering gaps
                stemWidthPixels = std::max(1.0f, (nextX - xPos) + 1.0f);
            }

            const float normalizedMagnitude = std::min(1.0f, std::max(0.0f, peak.magnitude));
            const auto barHeight = normalizedMagnitude * plotHeight;

            float midi = freqToMidi(peak.frequencyHz);
            float continuousChroma = std::fmod(midi, 12.0f);
            if (continuousChroma < 0.0f) continuousChroma += 12.0f;

            const juce::Colour color = Tone::getContinuousColor(continuousChroma);
            graphics.setColour(color);

            graphics.fillRoundedRectangle(
                xPos - (stemWidthPixels * 0.5f),
                plotHeight - barHeight,
                stemWidthPixels,
                barHeight,
                2.0f
            );
        }
    }
}

void RollStft::pumpSteam() {
    if (activePeaks.empty() || !steamImage.isValid()) {
        return;
    }

    const bool isHorizontal = processor.settings.isOrientationHorizontal;
    const int logicalWidth = isHorizontal ? getHeight() : getWidth();
    const int logicalHeight = isHorizontal ? getWidth() : getHeight();

    const int width = logicalWidth;
    const int height = std::max(1, logicalHeight - static_cast<int>(getLabelAreaHeight()));
    if (width <= 0 || height <= 0) return;

    constexpr int speedPx = static_cast<int>(steamSpeedPxPerFrame);

    // Advance the scroll offset and wrap it like a treadmill
    steamScrollOffset = (steamScrollOffset + speedPx) % height;

    // Calculate where in the image memory the new row should be drawn
    const int drawY = (height - speedPx + steamScrollOffset) % height;

    // Clear the new row first to prevent ghosting from previous treadmill cycles
    steamImage.clear(juce::Rectangle<int>(0, drawY, width, speedPx), juce::Colours::transparentBlack);

    juce::Graphics graphics(steamImage);

    const float sr = processor.getSampleRate() > 0.0 ? static_cast<float>(processor.getSampleRate()) : 44100.0f;
    const float binResHz = sr / 32768.0f;
    const float fWidth = static_cast<float>(width);
    const float midiRangeInv = 1.0f / (maxMidiNote - minMidiNote);
    const bool doDynamicStem = enableDynamicStemWidth;

    // Extreme Math Optimization: Pre-calculate the derivative of the MIDI scale
    // to bypass calling std::log2 multiple times per peak.
    const float derivativeFactor = fWidth * midiRangeInv * 17.3123405f * binResHz;

    for (const auto& peak : activePeaks) {
        if (peak.magnitude > 0.05f) {
            // Prevents rendering absolute silence noise

            // Inline the math for the primary position calculation
            const float midi = 69.0f + 12.0f * std::log2(peak.frequencyHz / 440.0f);
            const float xPos = fWidth * ((midi - minMidiNote) * midiRangeInv);

            // Fast bounds culling
            if (xPos >= -10.0f && xPos <= fWidth + 10.0f) {
                // Configurable razor-sharp stems for the Steam
                float stemWidthPixels = 4.0f;
                if (doDynamicStem) {
                    // Use the fast derivative approximation instead of another heavy log2
                    const float nextX = xPos + (derivativeFactor / peak.frequencyHz);
                    // +1.0f forces deliberate sub-pixel overlap to completely kill rendering gaps
                    stemWidthPixels = std::max(1.0f, (nextX - xPos) + 1.0f);
                }

                // Fast continuous modulus (replaces heavy std::fmod)
                float continuousChroma = midi;
                while (continuousChroma >= 12.0f) continuousChroma -= 12.0f;
                while (continuousChroma < 0.0f) continuousChroma += 12.0f;

                const juce::Colour baseColor = Tone::getContinuousColor(continuousChroma);
                constexpr float undimmingGain = 1.3f;

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

void RollStft::paintSteam(const juce::Graphics& graphics) const {
    if (!steamImage.isValid()) return;

    const bool isHorizontal = processor.settings.isOrientationHorizontal;
    const int logicalHeight = isHorizontal ? getWidth() : getHeight();

    const int height = std::max(1, logicalHeight - static_cast<int>(getLabelAreaHeight()));

    // Draw the two halves of the ring buffer to create a flawless infinite upward scroll
    graphics.drawImageAt(steamImage, 0, -steamScrollOffset);
    graphics.drawImageAt(steamImage, 0, height - steamScrollOffset);
}
