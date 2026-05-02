#include "EyePiano.h"
#include "../Tone.h"

EyePiano::EyePiano() {
    smoothedOctaveBins.resize(totalFoldedBins, 0.0);
}

void EyePiano::updateResults(const std::vector<double>& results) {
    if (results.size() == smoothedOctaveBins.size()) {
        std::ranges::copy(results, smoothedOctaveBins.begin());
        repaint();
    }
}

void EyePiano::resized() {
    const float w = static_cast<float>(getWidth());
    const float laneWidth = w / 12.0f;

    lanes.clear();
    for (int i = 0; i < 12; ++i) {
        bool isAccidental = (i == 1 || i == 3 || i == 6 || i == 8 || i == 10);
        lanes.push_back({i, isAccidental, static_cast<float>(i) * laneWidth, laneWidth, Tone::chromaticScale[static_cast<size_t>(i)].color});
    }
}

void EyePiano::paint(juce::Graphics& g) {
    const float h = static_cast<float>(getHeight());
    const float w = static_cast<float>(getWidth());
    const float laneWidth = w / 12.0f;
    const float binWidth = w / static_cast<float>(totalFoldedBins);
    
    // Offset to center bin 0 on the first pitch center line
    const float offset = (laneWidth * 0.5f) - (binWidth * 0.5f);

    // 1. Paint the "Frame" (Vertical lines at center of pitches, staggered)
    for (const auto& lane : lanes) {
        const float centerX = lane.x + lane.width * 0.5f;
        
        // Staggered: Accidentals lines are shorter
        const float lineTop = 0.0f;
        const float lineBottom = lane.isAccidental ? h * 0.7f : h;
        
        g.setColour(lane.color.withAlpha(0.2f));
        g.drawLine(centerX, lineTop, centerX, lineBottom, 1.0f);
        
        if (lane.isAccidental) {
            // Draw a subtle background for accidental lanes to enhance piano feel
            g.setColour(lane.color.withAlpha(0.05f));
            g.fillRect(lane.x, 0.0f, lane.width, h * 0.7f);
        }
    }

    // 2. Paint the bins (Spectral data)
    for (int i = 0; i < totalFoldedBins; ++i) {
        const float magnitude = static_cast<float>(smoothedOctaveBins[static_cast<size_t>(i)]);
        if (magnitude <= 0.001f) continue;

        const float chroma = static_cast<float>(i) / static_cast<float>(binsPerSemitone);
        
        float x = static_cast<float>(i) * binWidth + offset;
        if (x >= w) x -= w;
        if (x < 0.0f) x += w;

        // Determine height based on whether the bin falls into an accidental lane
        const int laneIndex = juce::jlimit(0, 11, static_cast<int>(std::floor(x / laneWidth)));
        const bool isAccidentalLane = (laneIndex == 1 || laneIndex == 3 || laneIndex == 6 || laneIndex == 8 || laneIndex == 10);

        const float barMaxHeight = isAccidentalLane ? h * 0.7f : h;
        const float barHeight = barMaxHeight * magnitude;
        
        // Draw from bottom up
        const float barTop = barMaxHeight - barHeight;

        const juce::Colour color = Tone::getContinuousColor(chroma);
        
        // Gradient fill for a "roll" feel
        g.setGradientFill(juce::ColourGradient::vertical(
            color.withAlpha(0.8f), barTop,
            color.withAlpha(0.2f), barMaxHeight
        ));
        g.fillRect(x, barTop, binWidth, barHeight);
        
        // Top highlight
        g.setColour(juce::Colours::white.withAlpha(std::min(1.0f, magnitude * 0.8f)));
        g.fillRect(x, barTop, binWidth, 1.5f);
    }
}
