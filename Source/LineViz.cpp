#include "LineViz.h"
#include "ColorPalette.h"
#include <algorithm>

LineViz::LineViz(PitchengaAudioProcessor& processor) : processor(processor) {}

void LineViz::updateResults(const std::vector<double>& results) {
    if (results.empty()) return;

    // 1. Initialize or Re-initialize the smoother if the number of bins changes
    if (smoother == nullptr || lastKnownSize != results.size()) {
        // We use 0.2 for the weight to match your existing octaveBinSmoother feel
        smoother = std::make_unique<ExpSmoother>(results.size(), 0.2);
        lastKnownSize = results.size();
    }

    displayMagnitudes = smoother->smooth(results);

    repaint();
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

    const int width = getWidth();
    const int height = getHeight();

    const float barWidth = static_cast<float>(width) / static_cast<float>(currentTotalBins);

    for (int i = 0; i < currentTotalBins; ++i) {
        if (i >= static_cast<int>(displayMagnitudes.size())) break;

        const double normalizedMagnitude = std::min(
            1.0,
            std::max(0.0, displayMagnitudes[static_cast<size_t>(i)])
        );
        const auto barHeight = static_cast<float>(normalizedMagnitude * height);

        const float chroma =
            static_cast<float>(i % currentBinsPerOctave) * 12.0f / static_cast<float>(currentBinsPerOctave);

        const juce::Colour color = ColorPalette::getContinuousColor(chroma);
        graphics.setColour(color);

        // Draw filled rectangle originating from the bottom boundary
        graphics.fillRect(
            static_cast<float>(i) * barWidth,
            static_cast<float>(height) - barHeight,
            barWidth,
            barHeight
        );
    }
}

void LineViz::resized() {
    paintFrame();
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

    // The exact pixel width of one full semitone
    const float semitoneWidth = static_cast<float>(getWidth()) / static_cast<float>(totalSemitones);
    const float halfHeight = static_cast<float>(getHeight()) * 0.5f;

    for (int i = 0; i < totalSemitones; ++i) {
        const int chroma = i % 12;

        // Identify standard "black" keys
        const bool isBlackKey = (chroma == 1 || chroma == 3 || chroma == 6 || chroma == 8 || chroma == 10);

        // 1. Find the exact pitch bin index for this semitone
        const float binIndex = static_cast<float>(i) * (static_cast<float>(currentBinsPerOctave) / 12.0f);

        // 2. Find the visual center of that specific pitch bin
        const float targetCenter = binIndex * barWidth + (barWidth * 0.5f);

        // 3. Shift the key left so its center perfectly aligns with the target
        const float x = targetCenter - (semitoneWidth * 0.5f);
        const float y = isBlackKey ? 0.0f : halfHeight;

        // The width remains strictly semitoneWidth so white keys NEVER overlap
        const juce::Rectangle<float> rect(x, y, semitoneWidth, halfHeight);

        const juce::Colour baseColor = ColorPalette::chromaticScale[static_cast<size_t>(chroma)].color;
        const juce::Colour color = baseColor.interpolatedWith(juce::Colours::black, 0.5f);

        graphics.setColour(baseColor);
        graphics.drawRect(rect, 1.0f);
    }
}
