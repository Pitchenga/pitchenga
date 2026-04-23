#include "RollCqt.h"
#include "../Tone.h"
#include <algorithm>

#include "Eye.h"

RollCqt::RollCqt(PitchengaAudioProcessor& proc) : processor(proc) {}

bool RollCqt::expand() {
    const int totalBins = static_cast<int>(displayMagnitudes.size());
    if (totalBins <= 0) return true;

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

    return false;
}

void RollCqt::updateResults(const std::vector<double>& results) {
    if (!isVisible()) return;

    if (results.empty()) return;
    displayMagnitudes = results;

    if (smoother == nullptr || lastKnownSize != results.size()) {
        smoother = std::make_unique<ExpSmoother>(results.size(), 0.5);
        lastKnownSize = results.size();
        accumulatedMagnitudes.assign(results.size(), 0.0);
    }

    if (expand()) return;

    displayMagnitudes = smoother->smooth(displayMagnitudes);

    // Accumulate maximum magnitudes over multiple audio frames to prevent transient dropping (flicker)
    if (accumulatedMagnitudes.size() == displayMagnitudes.size()) {
        for (size_t i = 0; i < displayMagnitudes.size(); ++i) {
            accumulatedMagnitudes[i] = std::max(accumulatedMagnitudes[i], displayMagnitudes[i]);
        }
    }

    repaint();
}

void RollCqt::resized() {
    const int width = getWidth();
    const int height = getHeight();

    if (width > 0 && height > 0) {
        const int plotHeight = std::max(1, height - static_cast<int>(getLabelAreaHeight()));
        // Initialize the rolling buffer whenever the window resizes
        steamImage = juce::Image(juce::Image::ARGB, width, plotHeight, true);
        lastPumpTimeMs = 0.0;
        currentScrollY = 0.0f;
        lastWrittenRow = 0;
        paintFrame();
    }
}

juce::Font RollCqt::getLabelFont() {
    return {
        juce::FontOptions(13.0f)
        .withStyle("Bold")
        .withName(juce::Font::getDefaultMonospacedFontName())
    };
}

float RollCqt::getLabelAreaHeight() {
    return juce::GlyphArrangement::getStringWidth(getLabelFont(), "Ww8") + 4.0f;
}

void RollCqt::paint(juce::Graphics& graphics) {
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

    currentTotalBins = static_cast<int>(displayMagnitudes.size());
    currentBinsPerOctave = currentTotalBins / PitchengaAudioProcessor::numOctaves;

    if (currentTotalBins <= 0 || currentBinsPerOctave <= 0 || displayMagnitudes.empty()) return;

    graphics.saveState();
    graphics.reduceClipRegion(0, 0, width, plotHeight);

    if (processor.settings.showSteam) {
        paintSteam(graphics);
    }

    if (processor.settings.showForrest) {
        paintBins(graphics);
    }

    graphics.restoreState();
}

void RollCqt::paintFrame() {
    const int width = getWidth();
    const int height = getHeight();
    if (width <= 0 || height <= 0) return;

    if (currentTotalBins <= 0 || currentBinsPerOctave <= 0) return;

    // Create a transparent image (the 'true' flag clears it to zero alpha)
    cachedFrame = juce::Image(juce::Image::ARGB, width, height, true);
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
    const int i,
    const float targetCenter,
    const float startY,
    const juce::Colour baseColor
) {
    if (i == 0) {
        // Not drawing a half label
        return;
    }
    constexpr int startMidiNote = 12;
    const int midiNote = i + startMidiNote;

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

void RollCqt::paintFrame(juce::Graphics& graphics) const {
    int totalOctaves = currentTotalBins / currentBinsPerOctave;
    if (totalOctaves <= 0) totalOctaves = PitchengaAudioProcessor::numOctaves;
    const int totalSemitones = totalOctaves * 12;

    if (totalSemitones <= 0) return;

    // The exact pixel width of one single CQT bin
    const float barWidth = static_cast<float>(getWidth()) / static_cast<float>(currentTotalBins);

    const auto totalHeight = static_cast<float>(getHeight());
    const float labelAreaHeight = getLabelAreaHeight();
    const float plotHeight = std::max(1.0f, totalHeight - labelAreaHeight);
    const float halfHeight = plotHeight * 0.5f;

    const juce::Font labelFont = getLabelFont();
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

void RollCqt::paintBins(juce::Graphics& graphics) const {
    const int width = getWidth();
    const float plotHeight = std::max(1.0f, static_cast<float>(getHeight()) - getLabelAreaHeight());

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
        currentScrollY += targetPixelsPerSecond * deltaSec;
    }

    const int currentIntY = static_cast<int>(currentScrollY);
    const int rowsToWrite = currentIntY - lastWrittenRow;

    if (rowsToWrite < 1) {
        repaint();
        return;
    }

    bool hasNewData = false;
    for (const double mag : accumulatedMagnitudes) {
        if (mag > 0.0) {
            hasNewData = true;
            break;
        }
    }

    if (hasNewData) {
        lastMagnitudes = accumulatedMagnitudes;
        std::ranges::fill(accumulatedMagnitudes, 0.0);
    }

    const int totalBins = static_cast<int>(lastMagnitudes.size());
    const int binsPerOctave = totalBins > 0 ? totalBins / PitchengaAudioProcessor::numOctaves : 1;
    const float barWidth = totalBins > 0 ? static_cast<float>(width) / static_cast<float>(totalBins) : 0.0f;

    if (lastWrittenRow + rowsToWrite > height) {
        const int firstPart = height - lastWrittenRow;
        const int secondPart = rowsToWrite - firstPart;
        steamImage.clear(juce::Rectangle<int>(0, lastWrittenRow, width, firstPart), juce::Colours::transparentBlack);
        steamImage.clear(juce::Rectangle<int>(0, 0, width, secondPart), juce::Colours::transparentBlack);
    } else {
        steamImage.clear(juce::Rectangle<int>(0, lastWrittenRow, width, rowsToWrite), juce::Colours::transparentBlack);
    }

    juce::Graphics g(steamImage);

    if (!lastMagnitudes.empty()) {
        for (int i = 0; i < rowsToWrite; ++i) {
            const int targetY = (lastWrittenRow + i) % height;
            for (int b = 0; b < totalBins; ++b) {
                if (const double magnitude = lastMagnitudes[static_cast<size_t>(b)]; magnitude > steamThreshold) {
                    const float chroma = static_cast<float>(b % binsPerOctave) * 12.0f / static_cast<float>(binsPerOctave);
                    const juce::Colour baseColor = Tone::getContinuousColor(chroma);
                    constexpr float undimmingGain = 1.1f;
                    const juce::Colour color = juce::Colours::black.interpolatedWith(
                        baseColor,
                        static_cast<float>(magnitude * undimmingGain)
                    );

                    g.setColour(color);
                    g.fillRect(
                        static_cast<float>(b) * barWidth,
                        static_cast<float>(targetY),
                        barWidth + 0.5f,
                        1.0f
                    );
                }
            }
        }
    }

    lastWrittenRow = currentIntY;
    repaint();
}

void RollCqt::paintSteam(const juce::Graphics& graphics) const {
    if (!steamImage.isValid()) return;

    const int height = std::max(1, getHeight() - static_cast<int>(getLabelAreaHeight()));

    const float exactScrollOffset = std::fmod(currentScrollY, static_cast<float>(height));

    juce::Graphics& g = const_cast<juce::Graphics&>(graphics);
    
    g.drawImageTransformed(steamImage, juce::AffineTransform::translation(0.0f, -exactScrollOffset));
    g.drawImageTransformed(steamImage, juce::AffineTransform::translation(0.0f, static_cast<float>(height) - exactScrollOffset));
}
