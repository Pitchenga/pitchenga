#include "RollCqt.h"
#include "../Tone.h"
#include <algorithm>

#include "Eye.h"
#include "../Common.h"

RollCqt::RollCqt(PitchengaAudioProcessor& proc) : processor(proc) {}

bool RollCqt::expand() {
    const int totalBins = static_cast<int>(displayMagnitudes.size());
    if (totalBins <= 0) return false;

    double* magnitudes = displayMagnitudes.data();

    // MAnalyzer achieves its clean look by simply dropping the visual floor.
    // Our raw data maps 0.0 to -90dB. A lot of low-level acoustic noise lives there.
    // By setting the visual floor to 0.35 (approx -58dB), the noise falls entirely off the screen,
    // leaving only the pure, distinct harmonic peaks.
    constexpr double visualFloor = 0.2;
    constexpr double rangeInv = 1.2 / (1.0 - visualFloor);

    for (int i = 0; i < totalBins; ++i) {
        if (const double magnitude = magnitudes[i]; magnitude <= visualFloor) {
            magnitudes[i] = 0.0;
        } else {
            // Stretch the remaining peaks back to the 0.0 - 1.0 range for drawing
            magnitudes[i] = (magnitude - visualFloor) * rangeInv;
        }
    }

    return true;
}

void RollCqt::updateResults(const std::vector<double>& results) {
    if (processor.settings.isUseRollStft
        || processor.settings.isFreezeRoll
        || results.empty()
        || !isVisible()
    ) {
        return;
    }

    displayMagnitudes = results;

    if (smoother == nullptr || lastKnownSize != results.size()) {
        smoother = std::make_unique<ExpSmoother>(results.size(), 0.5);
        lastKnownSize = results.size();
    }

    if (!expand()) return;

    displayMagnitudes = smoother->smooth(displayMagnitudes);

    if (processor.settings.isShowSteam) {
        pumpSteam();
    }

    repaint();
}

void RollCqt::resized() {
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

float RollCqt::getLabelAreaHeight() const {
    //fixme: Unify range and hide labels when adjacent to tuner
    // if (!processor.settings.isShowRollLabels()) {
        // return 0.0f;
    // }
    return juce::GlyphArrangement::getStringWidth(Common::getLabelFont(), "Ww8") + 4.0f;
}

void RollCqt::paint(juce::Graphics& graphics) {
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

    if (cachedFrame.isValid()) {
        graphics.drawImageAt(cachedFrame, 0, 0);
    }

    currentTotalBins = static_cast<int>(displayMagnitudes.size());
    currentBinsPerOctave = currentTotalBins / PitchengaAudioProcessor::numOctaves;

    if (currentTotalBins <= 0 || currentBinsPerOctave <= 0 || displayMagnitudes.empty()) return;

    graphics.saveState();
    
    if (isHorizontal) {
        graphics.addTransform(juce::AffineTransform(0, 1, 0, -1, 0, physicalHeight));
    }
    
    graphics.reduceClipRegion(0, 0, logicalWidth, plotHeight);

    if (processor.settings.isShowSteam) {
        paintSteam(graphics);
    }

    if (processor.settings.isShowForrest) {
        paintForrest(graphics);
    }

    graphics.restoreState();
}

void RollCqt::buildFrame() {
    const bool isHorizontal = processor.settings.isOrientationHorizontal;
    const int logicalWidth = isHorizontal ? getHeight() : getWidth();
    const int logicalHeight = isHorizontal ? getWidth() : getHeight();
    if (logicalWidth <= 0 || logicalHeight <= 0) return;

    if (currentTotalBins <= 0 || currentBinsPerOctave <= 0) return;

    // Create a transparent image (the 'true' flag clears it to zero alpha)
    cachedFrame = juce::Image(juce::Image::ARGB, logicalWidth, logicalHeight, true);
    juce::Graphics graphics(cachedFrame);
    paintFrame(graphics);
}

juce::String RollCqt::getNoteName(const int midiNote) {
    int chroma = midiNote % 12;
    if (chroma < 0) chroma += 12;
    const int octave = midiNote / 12 - 1;
    return Tone::chromaticScale[static_cast<size_t>(chroma)].toneName + juce::String(octave);
}

void RollCqt::paintLabel(
    juce::Graphics& graphics,
    const float labelHeight,
    const float maxTextWidth,
    const int binIndex,
    const float targetCenter,
    const float startY,
    const juce::Colour baseColor,
    const bool isHorizontal
) {
    if (binIndex == 0) {
        // Not drawing a half label
        return;
    }
    constexpr int startMidiNote = 12;
    const int midiNote = binIndex + startMidiNote;

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

void RollCqt::paintFrame(juce::Graphics& graphics) const {
    const bool isHorizontal = processor.settings.isOrientationHorizontal;
    const int logicalWidth = isHorizontal ? getHeight() : getWidth();
    const int logicalHeight = isHorizontal ? getWidth() : getHeight();

    int totalOctaves = currentTotalBins / currentBinsPerOctave;
    if (totalOctaves <= 0) totalOctaves = PitchengaAudioProcessor::numOctaves;
    const int totalSemitones = totalOctaves * 12;

    if (totalSemitones <= 0) return;

    // The exact pixel width of one single CQT bin
    const float barWidth = static_cast<float>(logicalWidth) / static_cast<float>(currentTotalBins);

    const auto totalHeight = static_cast<float>(logicalHeight);
    const float labelAreaHeight = getLabelAreaHeight();
    const float plotHeight = std::max(1.0f, totalHeight - labelAreaHeight);

    const juce::Font labelFont = Common::getLabelFont();
    graphics.setFont(labelFont);
    const float labelHeight = labelFont.getHeight();
    const float maxTextWidth = juce::GlyphArrangement::getStringWidth(labelFont, "Ww8");

    for (int i = 0; i < totalSemitones; ++i) {
        const int chroma = i % 12;

        //fixme: move to ToneName
        // Identify standard "black" keys
        const bool isBlackKey = chroma == 1 || chroma == 3 || chroma == 6 || chroma == 8 || chroma == 10;

        // Find the exact pitch bin index for this semitone
        const float binIndex = static_cast<float>(i) * (static_cast<float>(currentBinsPerOctave) / 12.0f);

        // Find the visual center of that specific pitch bin
        const float targetCenter = binIndex * barWidth + barWidth * 0.5f;

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

        //fixme: Unify range and hide labels when adjacent to tuner
        paintLabel(graphics, labelHeight, maxTextWidth, i, targetCenter, totalHeight, baseColor, processor.settings.isOrientationHorizontal);
    }
}

void RollCqt::paintForrest(juce::Graphics& graphics) const {
    const bool isHorizontal = processor.settings.isOrientationHorizontal;
    const int logicalWidth = isHorizontal ? getHeight() : getWidth();
    const int logicalHeight = isHorizontal ? getWidth() : getHeight();

    const int width = logicalWidth;
    const float plotHeight = std::max(1.0f, static_cast<float>(logicalHeight) - getLabelAreaHeight());

    const float barWidth = static_cast<float>(width) / static_cast<float>(currentTotalBins);

    for (int i = 0; i < currentTotalBins; ++i) {
        if (i >= static_cast<int>(displayMagnitudes.size())) break;

        std::vector<double>::value_type magnitude = displayMagnitudes[static_cast<size_t>(i)];
        const double normalizedMagnitude = std::min(
            1.0,
            std::max(0.0, magnitude)
        );
        const auto barHeight = static_cast<float>(normalizedMagnitude) * plotHeight;

        const float chroma =
            static_cast<float>(i % currentBinsPerOctave) * 12.0f / static_cast<float>(currentBinsPerOctave);

        const juce::Colour color = Tone::getContinuousColor(chroma);
        graphics.setColour(color);

        graphics.fillRect(
            static_cast<float>(i) * barWidth,
            plotHeight - barHeight,
            barWidth + 0.5f,
            barHeight
        );
    }
}

void RollCqt::pumpSteam() {
    if (displayMagnitudes.empty() || !steamImage.isValid()) {
        return;
    }

    const bool isHorizontal = processor.settings.isOrientationHorizontal;
    const int logicalWidth = isHorizontal ? getHeight() : getWidth();
    const int logicalHeight = isHorizontal ? getWidth() : getHeight();

    const int width = logicalWidth;
    const int height = std::max(1, logicalHeight - static_cast<int>(getLabelAreaHeight()));
    if (width <= 0 || height <= 0) return;

    const int speedPx = static_cast<int>(steamSpeedPxPerFrame);

    // Advance the scroll offset and wrap it like a treadmill
    steamScrollOffset = (steamScrollOffset + speedPx) % height;

    // Calculate where in the image memory the new row should be drawn
    const int drawY = (height - speedPx + steamScrollOffset) % height;

    // Clear the new row first to prevent ghosting from previous treadmill cycles
    steamImage.clear(juce::Rectangle(0, drawY, width, speedPx), juce::Colours::transparentBlack);

    juce::Graphics graphics(steamImage);

    const int totalBins = static_cast<int>(displayMagnitudes.size());
    const int binsPerOctave = totalBins / PitchengaAudioProcessor::numOctaves;
    const float barWidth = static_cast<float>(width) / static_cast<float>(totalBins);

    for (int i = 0; i < totalBins; ++i) {
        if (const double magnitude = displayMagnitudes[static_cast<size_t>(i)]; magnitude > steamThreshold) {
            const float chroma = static_cast<float>(i % binsPerOctave) * 12.0f / static_cast<float>(binsPerOctave);
            const juce::Colour baseColor = Tone::getContinuousColor(chroma);
            constexpr float undimmingGain = 1.1f;
            const juce::Colour color = juce::Colours::black.interpolatedWith(
                baseColor,
                static_cast<float>(magnitude * undimmingGain)
            );

            graphics.setColour(color);
            graphics.fillRect(
                static_cast<float>(i) * barWidth,
                static_cast<float>(drawY),
                barWidth + 0.5f,
                steamSpeedPxPerFrame
            );
        }
    }
}

void RollCqt::paintSteam(const juce::Graphics& graphics) const {
    if (!steamImage.isValid()) return;

    const bool isHorizontal = processor.settings.isOrientationHorizontal;
    const int logicalHeight = isHorizontal ? getWidth() : getHeight();

    const int height = std::max(1, logicalHeight - static_cast<int>(getLabelAreaHeight()));

    // Draw the two halves of the ring buffer to create a flawless infinite upward scroll
    graphics.drawImageAt(steamImage, 0, -steamScrollOffset);
    graphics.drawImageAt(steamImage, 0, height - steamScrollOffset);
}
