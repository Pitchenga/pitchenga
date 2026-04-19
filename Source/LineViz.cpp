#include "LineViz.h"
#include "ColorPalette.h"
#include <algorithm>

#include "CircleViz.h"

LineViz::LineViz(PitchengaAudioProcessor& proc) : processor(proc) {}

bool LineViz::expand() {
    const int totalBins = static_cast<int>(displayMagnitudes.size());
    if (totalBins <= 0) return true;

    double* dataPointer = displayMagnitudes.data();

    // MAnalyzer achieves its clean look by simply dropping the visual floor.
    // Our raw data maps 0.0 to -90dB. A lot of low-level acoustic noise lives there.
    // By setting the visual floor to 0.35 (approx -58dB), the noise falls entirely off the screen,
    // leaving only the pure, distinct harmonic peaks.
    constexpr double visualFloor = 0.2;
    constexpr double rangeInv = 1.2 / (1.0 - visualFloor);

    for (int i = 0; i < totalBins; ++i) {
        double val = dataPointer[i];

        if (val <= visualFloor) {
            dataPointer[i] = 0.0;
        } else {
            // Stretch the remaining peaks back to the 0.0 - 1.0 range for drawing
            dataPointer[i] = (val - visualFloor) * rangeInv;
        }
    }

    return false;
}

void LineViz::updateResults(const std::vector<double>& results) {
    if (results.empty()) return;
    displayMagnitudes = results;

    if (smoother == nullptr || lastKnownSize != results.size()) {
        smoother = std::make_unique<ExpSmoother>(results.size(), 0.5);
        lastKnownSize = results.size();
    }

    if (expand()) return;
    advanceBubbles();
    displayMagnitudes = smoother->smooth(displayMagnitudes);

    repaint();
}

void LineViz::resized() {
    const int width = getWidth();
    const int height = getHeight();

    if (width > 0 && height > 0) {
        // Initialize the rolling buffer whenever the window resizes
        bubblesImage = juce::Image(juce::Image::ARGB, width, height, true);
        bubblesScrollOffset = 0;
        paintFrame();
    }
}

void LineViz::paint(juce::Graphics& graphics) {
    if (!cachedFrame.isValid()) {
        paintFrame(); // Generates it if the engine wasn't ready during resized()
    }
    if (cachedFrame.isValid()) {
        graphics.drawImageAt(cachedFrame, 0, 0);
    }

    currentTotalBins = static_cast<int>(displayMagnitudes.size());
    currentBinsPerOctave = currentTotalBins / PitchengaAudioProcessor::numOctaves;

    if (currentTotalBins <= 0 || currentBinsPerOctave <= 0 || displayMagnitudes.empty()) return;

    paintBubbles(graphics);
    // paintBins(graphics);
}

void LineViz::paintFrame() {
    const int width = getWidth();
    const int height = getHeight();
    if (width <= 0 || height <= 0) return;

    if (currentTotalBins <= 0 || currentBinsPerOctave <= 0) return;

    // Create a transparent image (the 'true' flag clears it to zero alpha)
    cachedFrame = juce::Image(juce::Image::ARGB, width, height, true);
    juce::Graphics graphics(cachedFrame);
    paintFrame(graphics);
}

void LineViz::paintFrame(juce::Graphics& graphics) const {
    int totalOctaves = currentTotalBins / currentBinsPerOctave;
    if (totalOctaves <= 0) totalOctaves = PitchengaAudioProcessor::numOctaves;
    const int totalSemitones = totalOctaves * 12;

    if (totalSemitones <= 0) return;

    // The exact pixel width of one single CQT bin
    const float barWidth = static_cast<float>(getWidth()) / static_cast<float>(currentTotalBins);

    const auto height = static_cast<float>(getHeight());
    const float halfHeight = height * 0.5f;

    for (int i = 0; i < totalSemitones; ++i) {
        const int chroma = i % 12;

        //fixme: move to Tone
        // Identify standard "black" keys
        const bool isBlackKey = (chroma == 1 || chroma == 3 || chroma == 6 || chroma == 8 || chroma == 10);

        // 1. Find the exact pitch bin index for this semitone
        const float binIndex = static_cast<float>(i) * (static_cast<float>(currentBinsPerOctave) / 12.0f);

        // 2. Find the visual center of that specific pitch bin
        const float targetCenter = binIndex * barWidth + barWidth * 0.5f;

        // 3. Route the line to the top half (black keys) or bottom half (white keys)
        constexpr float startY = 0.0f;
        const float endY = isBlackKey ? halfHeight : height;

        const juce::Colour baseColor = ColorPalette::chromaticScale[static_cast<size_t>(chroma)].color;
        const juce::Colour color = juce::Colours::black.interpolatedWith(baseColor, 0.3f);

        graphics.setColour(color);

        // Draw a strict 1px vertical line exactly at the calculated center
        graphics.drawLine(targetCenter, startY, targetCenter, endY, 1.0f);
    }
}

void LineViz::paintBins(juce::Graphics& graphics) const {
    const int width = getWidth();
    const int height = getHeight();

    const float barWidth = static_cast<float>(width) / static_cast<float>(currentTotalBins);

    for (int i = 0; i < currentTotalBins; ++i) {
        if (i >= static_cast<int>(displayMagnitudes.size())) break;

        std::vector<double>::value_type magnitude = displayMagnitudes[static_cast<size_t>(i)];
        const double normalizedMagnitude = std::min(
            1.0,
            std::max(0.0, magnitude)
        );
        const auto barHeight = static_cast<float>(normalizedMagnitude * height);

        const float chroma =
            static_cast<float>(i % currentBinsPerOctave) * 12.0f / static_cast<float>(currentBinsPerOctave);

        const juce::Colour color = ColorPalette::getContinuousColor(chroma);
        graphics.setColour(color);

        graphics.fillRect(
            static_cast<float>(i) * barWidth,
            static_cast<float>(height) - barHeight,
            barWidth + 0.5f,
            barHeight
        );
    }
}


void LineViz::advanceBubbles() {
    if (displayMagnitudes.empty() || !bubblesImage.isValid()) return;

    const int width = getWidth();
    const int height = getHeight();
    if (width <= 0 || height <= 0) return;

    const int speedPx = static_cast<int>(bubblesSpeedPxPerFrame);

    // Advance the scroll offset and wrap it like a treadmill
    bubblesScrollOffset = (bubblesScrollOffset + speedPx) % height;

    // Calculate where in the image memory the new row should be drawn
    const int drawY = (height - speedPx + bubblesScrollOffset) % height;

    // Native JUCE memory wipe: clears the specific row to completely transparent
    bubblesImage.clear(juce::Rectangle<int>(0, drawY, width, speedPx), juce::Colours::transparentBlack);

    juce::Graphics graphics(bubblesImage);

    const int totalBins = static_cast<int>(displayMagnitudes.size());
    const int binsPerOctave = totalBins / PitchengaAudioProcessor::numOctaves;
    const float barWidth = static_cast<float>(width) / static_cast<float>(totalBins);

    for (int i = 0; i < totalBins; ++i) {
        if (const double magnitude = displayMagnitudes[static_cast<size_t>(i)]; magnitude > bubbleThreshold) {
            const float chroma = static_cast<float>(i % binsPerOctave) * 12.0f / static_cast<float>(binsPerOctave);
            const juce::Colour baseColor = ColorPalette::getContinuousColor(chroma);
            const juce::Colour color = juce::Colours::black.interpolatedWith(baseColor, static_cast<float>(magnitude * 1.5f));

            graphics.setColour(color);
            graphics.fillRect(
                static_cast<float>(i) * barWidth,
                static_cast<float>(drawY),
                barWidth + 0.5f,
                bubblesSpeedPxPerFrame
            );
        }
    }
}

void LineViz::paintBubbles(juce::Graphics& graphics) const {
    if (!bubblesImage.isValid()) return;

    const int height = getHeight();

    // Draw the two halves of the ring buffer to create a flawless infinite upward scroll
    graphics.drawImageAt(bubblesImage, 0, -bubblesScrollOffset);
    graphics.drawImageAt(bubblesImage, 0, height - bubblesScrollOffset);
}