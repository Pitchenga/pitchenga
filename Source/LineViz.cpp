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
    if (engine) {
        currentTotalBins = engine->getTotalBins();
        currentBinsPerOctave = engine->getBinsPerOctave();
    } else {
        currentTotalBins = static_cast<int>(displayMagnitudes.size());
        if (PitchengaAudioProcessor::numOctaves > 0) {
            currentBinsPerOctave = currentTotalBins / PitchengaAudioProcessor::numOctaves;
        } else {
            currentBinsPerOctave = 12; // Fallback
        }
    }

    if (currentTotalBins <= 0 || currentBinsPerOctave <= 0 || displayMagnitudes.empty())
        return;

    const int width = getWidth();
    const int height = getHeight();

    const float barWidth = static_cast<float>(width) / static_cast<float>(currentTotalBins);
    
    for (int i = 0; i < currentTotalBins; ++i) {
        if (i >= static_cast<int>(displayMagnitudes.size())) break;

        // Add a multiplier to make weak signals more visible as requested by the specification
        constexpr double scalingMultiplier = 1.5;
        const double normalizedMagnitude = std::min(1.0, std::max(0.0, displayMagnitudes[static_cast<size_t>(i)] * scalingMultiplier));
        const auto barHeight = static_cast<float>(normalizedMagnitude * height);
        
        const float chroma = static_cast<float>(i % currentBinsPerOctave) * 12.0f / static_cast<float>(currentBinsPerOctave);
        const juce::Colour color = ColorPalette::getContinuousColor(chroma);
        
        graphics.setColour(color);
        // Draw filled rectangle originating from the bottom boundary
        graphics.fillRect(static_cast<float>(i) * barWidth,
                   static_cast<float>(height) - barHeight, 
                   barWidth, 
                   barHeight);
    }
}
