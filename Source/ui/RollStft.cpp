#include "RollStft.h"
#include "../Tone.h"
#include <algorithm>
#include <cmath>

RollStft::RollStft(PitchengaAudioProcessor& proc) : processor(proc) {}

void RollStft::updateResults(const std::vector<SpectralPeak>& peaks) {
    if (processor.settings.freezeRoll || !isVisible()) return;

    activePeaks = peaks;

    if (processor.settings.showSteam) {
        pumpSteam();
    }

    repaint();
}

void RollStft::resized() {
    const int width = getWidth();
    const int height = getHeight();

    if (width > 0 && height > 0) {
        const int plotHeight = std::max(1, height - static_cast<int>(getLabelAreaHeight()));
        // Initialize the rolling buffer whenever the window resizes
        steamImage = juce::Image(juce::Image::ARGB, width, plotHeight, true);
        steamScrollOffset = 0;
        lastPumpSamples = 0;
        subPixelAccumulator = 0.0f;
        paintFrame();
    }
}

juce::Font RollStft::getLabelFont() {
    return {
        juce::FontOptions(13.0f)
        .withStyle("Bold")
        .withName(juce::Font::getDefaultMonospacedFontName())
    };
}

float RollStft::getLabelAreaHeight() {
    return juce::GlyphArrangement::getStringWidth(getLabelFont(), "Ww8") + 4.0f;
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
        paintFrame(); // Generates it if the engine wasn't ready during resized()
    }

    const int width = getWidth();
    const int height = getHeight();
    const float labelAreaHeight = getLabelAreaHeight();
    const int plotHeight = std::max(1, height - static_cast<int>(labelAreaHeight));

    if (cachedFrame.isValid()) {
        graphics.drawImageAt(cachedFrame, 0, 0);
    }

    graphics.saveState();
    graphics.reduceClipRegion(0, 0, width, plotHeight);

    if (processor.settings.showSteam) {
        paintSteam(graphics);
    }

    if (!activePeaks.empty()) {
        if (processor.settings.showForrest) {
            paintPeaks(graphics);
        }
    }

    graphics.restoreState();
}

void RollStft::paintFrame() {
    const int width = getWidth();
    const int height = getHeight();
    if (width <= 0 || height <= 0) return;

    // Create a transparent image (the 'true' flag clears it to zero alpha)
    cachedFrame = juce::Image(juce::Image::ARGB, width, height, true);
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
    const juce::Colour baseColor
) {
    //fixme: Un-hardcode
    if (midiNote == minMidiNote || midiNote == 108) {
        // Not drawing a half label
        return;
    }

    const juce::String name = getNoteName(midiNote);

    graphics.setColour(baseColor);
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

void RollStft::paintFrame(juce::Graphics& graphics) const {
    const auto totalHeight = static_cast<float>(getHeight());
    const float labelAreaHeight = getLabelAreaHeight();
    const float plotHeight = std::max(1.0f, totalHeight - labelAreaHeight);
    const float halfHeight = plotHeight * 0.5f;

    const juce::Font labelFont = getLabelFont();
    graphics.setFont(labelFont);
    const float labelHeight = labelFont.getHeight();
    const float maxTextWidth = juce::GlyphArrangement::getStringWidth(labelFont, "Ww8");

    const int startMidi = static_cast<int>(std::ceil(minMidiNote));
    const int endMidi = static_cast<int>(std::floor(maxMidiNote));

    for (int i = startMidi; i <= endMidi; ++i) {
        const int chroma = i % 12;

        // fixme: move to ToneName
        // Identify standard "black" keys
        const bool isBlackKey = chroma == 1 || chroma == 3 || chroma == 6 || chroma == 8 || chroma == 10;

        const float hz = 440.0f * std::pow(2.0f, (static_cast<float>(i) - 69.0f) / 12.0f);
        const float targetCenter = frequencyToX(hz, static_cast<float>(getWidth()));

        // Route the line to the top half (black keys) or bottom half (white keys)
        const float startY = 0.0f;
        const float endY = isBlackKey ? halfHeight : plotHeight;

        const juce::Colour baseColor = Tone::chromaticScale[static_cast<size_t>(chroma)].color;
        const juce::Colour gridColor = juce::Colours::black.interpolatedWith(baseColor, 0.2f);
        graphics.setColour(gridColor);
        graphics.drawLine(targetCenter, startY, targetCenter, endY, 1.0f);

        paintLabel(graphics, labelHeight, maxTextWidth, i, targetCenter, totalHeight, baseColor);
    }
}

void RollStft::paintPeaks(juce::Graphics& graphics) const {
    const int width = getWidth();
    const float plotHeight = std::max(1.0f, static_cast<float>(getHeight()) - getLabelAreaHeight());

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
    if (activePeaks.empty() || !steamImage.isValid()) return;

    const int width = getWidth();
    const int height = std::max(1, getHeight() - static_cast<int>(getLabelAreaHeight()));
    if (width <= 0 || height <= 0) return;

    // Use absolute audio samples processed as the master timeline clock.
    // This perfectly synchronizes side-by-side instances and eliminates OS timer drifting!
    const int64_t currentSamples = processor.getTotalNumSamplesProcessed();

    // Reset if it's the first run or if the audio engine resets the sample counter
    if (lastPumpSamples == 0 || currentSamples < lastPumpSamples) {
        lastPumpSamples = currentSamples;
        return;
    }

    const int64_t deltaSamples = currentSamples - lastPumpSamples;
    lastPumpSamples = currentSamples;

    const double sampleRate = processor.getSampleRate() > 0.0 ? processor.getSampleRate() : 44100.0;
    const float deltaSec = static_cast<float>(deltaSamples) / static_cast<float>(sampleRate);

    subPixelAccumulator += targetPixelsPerSecond * deltaSec;
    const int speedPx = static_cast<int>(subPixelAccumulator);

    if (speedPx < 1) return;

    subPixelAccumulator -= static_cast<float>(speedPx);

    // Advance the scroll offset and wrap it like a treadmill
    steamScrollOffset = (steamScrollOffset + speedPx) % height;

    // Calculate where in the image memory the new row should be drawn
    const int drawY = (height - speedPx + steamScrollOffset) % height;

    if (activePeaks.empty()) {
        juce::Image::BitmapData bitmapData(steamImage, juce::Image::BitmapData::writeOnly);
        for (int yOffset = 0; yOffset < speedPx; ++yOffset) {
            const int targetY = drawY + yOffset;
            for (int x = 0; x < width; ++x) {
                bitmapData.setPixelColour(x, targetY, juce::Colours::black);
            }
        }
        return;
    }

    const float fWidth = static_cast<float>(width);
    const float sr = static_cast<float>(sampleRate);
    const float binResHz = sr / 32768.0f;
    const float midiRangeInv = 1.0f / (maxMidiNote - minMidiNote);
    const bool doDynamicStem = enableDynamicStemWidth;

    // Extreme Math Optimization: Pre-calculate the derivative of the MIDI scale
    // to bypass calling std::log2 multiple times per peak.
    const float derivativeFactor = fWidth * midiRangeInv * 17.3123405f * binResHz;

    std::vector<juce::Colour> pixelRow(static_cast<size_t>(width), juce::Colours::black);

    for (const auto& peak : activePeaks) {
        if (peak.rawMagnitude > 0.05f) {
            // Inline the math for the primary position calculation
            const float midi = 69.0f + 12.0f * std::log2(peak.frequencyHz / 440.0f);
            const float normX = (midi - minMidiNote) * midiRangeInv;
            const float xPos = normX * fWidth;

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
                constexpr float undimmingGain = 1.6f;
                const float clampedMag = std::min(1.0f, peak.rawMagnitude * undimmingGain);
                const juce::Colour color = juce::Colours::black.interpolatedWith(baseColor, clampedMag);

                const int startX = std::max(0, static_cast<int>(xPos - stemWidthPixels * 0.5f));
                const int endX = std::min(width - 1, static_cast<int>(xPos + stemWidthPixels * 0.5f));

                for (int x = startX; x <= endX; ++x) {
                    pixelRow[static_cast<size_t>(x)] = color;
                }
            }
        }
    }

    // Directly wipe and paint the specific row in memory via pixel pointer
    juce::Image::BitmapData bitmapData(steamImage, juce::Image::BitmapData::writeOnly);
    for (int yOffset = 0; yOffset < speedPx; ++yOffset) {
        const int targetY = drawY + yOffset;
        for (int x = 0; x < width; ++x) {
            bitmapData.setPixelColour(x, targetY, pixelRow[static_cast<size_t>(x)]);
        }
    }
}

void RollStft::paintSteam(const juce::Graphics& graphics) const {
    if (!steamImage.isValid()) return;

    const int height = std::max(1, getHeight() - static_cast<int>(getLabelAreaHeight()));

    // Draw the two halves of the ring buffer to create a flawless infinite upward scroll
    graphics.drawImageAt(steamImage, 0, -steamScrollOffset);
    graphics.drawImageAt(steamImage, 0, height - steamScrollOffset);
}
