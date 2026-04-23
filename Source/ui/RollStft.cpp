#include "RollStft.h"
#include "../Tone.h"
#include <algorithm>
#include <cmath>

RollStft::RollStft(PitchengaAudioProcessor& proc) : processor(proc) {}

void RollStft::updateResults(const std::vector<SpectralPeak>& peaks) {
    if (processor.settings.freezeRoll || !isVisible()) return;

    // activePeaks is still tracked natively for the foreground paintPeaks layer
    activePeaks = peaks;
}

void RollStft::resized() {
    const int width = getWidth();
    const int height = getHeight();

    if (width > 0 && height > 0) {
        const int plotHeight = std::max(1, height - static_cast<int>(getLabelAreaHeight()));
        // Initialize the rolling buffer whenever the window resizes
        // Restored to ARGB so transparentBlack clears correctly without obscuring the background grid
        steamImage = juce::Image(juce::Image::ARGB, width, plotHeight, true);
        lastPumpTimeMs = 0.0;
        subPixelAccumulator = 0.0f;
        steamScrollOffset = 0;
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
        paintFrame();
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

        const float startY = 0.0f;
        const float endY = plotHeight;

        const juce::Colour baseColor = Tone::chromaticScale[static_cast<size_t>(chroma)].color;
        const juce::Colour gridColor = juce::Colours::black.interpolatedWith(baseColor, 0.2f);
        graphics.setColour(gridColor);

        if (isBlackKey) {
            const float dashLengths[] = { 4.0f, 4.0f };
            graphics.drawDashedLine(juce::Line<float>(targetCenter, startY, targetCenter, endY), dashLengths, 2, 1.0f);
        } else {
            graphics.drawLine(targetCenter, startY, targetCenter, endY, 1.0f);
        }

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

void RollStft::pumpSteam(const std::vector<juce::Colour>& pixelRow, bool hasNewData) {
    if (!steamImage.isValid()) return;

    const int width = getWidth();
    const int height = std::max(1, getHeight() - static_cast<int>(getLabelAreaHeight()));
    if (width <= 0 || height <= 0) return;

    const double now = juce::Time::getMillisecondCounterHiRes();
    if (lastPumpTimeMs == 0.0) {
        lastPumpTimeMs = now;
        return;
    }

    float deltaSec = static_cast<float>(now - lastPumpTimeMs) / 1000.0f;
    lastPumpTimeMs = now;

    if (deltaSec > 0.1f) deltaSec = 0.0f;

    if (deltaSec > 0.0f) {
        subPixelAccumulator += targetPixelsPerSecond * deltaSec;
    }

    int speedPx = static_cast<int>(subPixelAccumulator);

    if (speedPx < 1) {
        repaint();
        return;
    }

    if (speedPx > height) {
        speedPx = height;
        subPixelAccumulator = 0.0f;
    } else {
        subPixelAccumulator -= static_cast<float>(speedPx);
    }

    const int drawY = steamScrollOffset;
    steamScrollOffset = (steamScrollOffset + speedPx) % height;

    juce::Graphics g(steamImage);
    
    if (drawY + speedPx > height) {
        const int firstPart = height - drawY;
        const int secondPart = speedPx - firstPart;
        steamImage.clear(juce::Rectangle<int>(0, drawY, width, firstPart), juce::Colours::transparentBlack);
        steamImage.clear(juce::Rectangle<int>(0, 0, width, secondPart), juce::Colours::transparentBlack);
    } else {
        steamImage.clear(juce::Rectangle<int>(0, drawY, width, speedPx), juce::Colours::transparentBlack);
    }

    if (hasNewData && !pixelRow.empty() && pixelRow.size() == static_cast<size_t>(width)) {
        juce::Image::BitmapData bitmapData(steamImage, juce::Image::BitmapData::writeOnly);
        for (int i = 0; i < speedPx; ++i) {
            const int targetY = (drawY + i) % height;
            for (int x = 0; x < width; ++x) {
                bitmapData.setPixelColour(x, targetY, pixelRow[static_cast<size_t>(x)]);
            }
        }
    }

    repaint();
}

void RollStft::paintSteam(const juce::Graphics& graphics) const {
    if (!steamImage.isValid()) return;

    const int height = std::max(1, getHeight() - static_cast<int>(getLabelAreaHeight()));

    const float exactScrollOffset = static_cast<float>(steamScrollOffset) + subPixelAccumulator;

    juce::Graphics& g = const_cast<juce::Graphics&>(graphics);
    
    g.drawImageTransformed(steamImage, juce::AffineTransform::translation(0.0f, -exactScrollOffset));
    g.drawImageTransformed(steamImage, juce::AffineTransform::translation(0.0f, static_cast<float>(height) - exactScrollOffset));
}
