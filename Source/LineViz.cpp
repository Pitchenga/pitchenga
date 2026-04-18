#include "LineViz.h"
#include "ColorPalette.h"
#include <algorithm>

LineViz::LineViz(PitchengaAudioProcessor& proc) : processor(proc) {}

void LineViz::updateResults(const std::vector<double>& results) {
    if (results.empty()) return;

    if (smoother == nullptr || lastKnownSize != results.size()) {
        smoother = std::make_unique<ExpSmoother>(results.size(), 0.1);
        lastKnownSize = results.size();
    }

    displayMagnitudes = smoother->smooth(results);

    const int totalBins = static_cast<int>(displayMagnitudes.size());
    if (totalBins <= 0) return;

    double* dataPointer = displayMagnitudes.data();

    // --- 2. Per-Octave Ranking (Multiband Gating) ---
    const int binsPerOct = totalBins / PitchengaAudioProcessor::numOctaves;

    struct BinData {
        int index;
        float velocity;
    };

    std::vector<BinData> sortedBins(static_cast<size_t>(binsPerOct));
    std::vector<int> binOrders(static_cast<size_t>(binsPerOct));

    for (int oct = 0; oct < PitchengaAudioProcessor::numOctaves; ++oct) {
        const int startIndex = oct * binsPerOct;
        const int endIndex = std::min(startIndex + binsPerOct, totalBins);
        const int currentOctaveBins = endIndex - startIndex;

        if (currentOctaveBins <= 0) break;

        for (int i = 0; i < currentOctaveBins; ++i) {
            sortedBins[static_cast<size_t>(i)] = {i, static_cast<float>(dataPointer[startIndex + i])};
        }

        std::ranges::sort(
            sortedBins.begin(),
            sortedBins.begin() + currentOctaveBins,
            [](const BinData& a, const BinData& b) {
                return a.velocity < b.velocity;
            }
        );

        // --- NEW: Dynamic Octave Normalization ---
        // Find the absolute highest peak in this specific octave
        const float maxOctaveVelocity = sortedBins[static_cast<size_t>(currentOctaveBins - 1)].velocity;

        // If the tilt pushed this peak past 1.0 calculate how much to scale the whole octave down.
        // If it is below 1.0, leave it alone (1.0f multiplier).
        const float octaveGainReduction = maxOctaveVelocity > 1.0f ? (1.0f / maxOctaveVelocity) : 1.0f;

        for (int rank = 0; rank < currentOctaveBins; ++rank) {
            binOrders[static_cast<size_t>(sortedBins[static_cast<size_t>(rank)].index)] = rank;
        }

        const float rankMultiplier = 1.0f / static_cast<float>(currentOctaveBins > 1 ? currentOctaveBins - 1 : 1);

        for (int i = 0; i < currentOctaveBins; ++i) {
            const auto rawVelocity = static_cast<float>(dataPointer[startIndex + i]);
            const float rankContrast = static_cast<float>(binOrders[static_cast<size_t>(i)]) * rankMultiplier;

            // Apply the gain reduction so the whole octave shrinks proportionally to fit
            float renderVelocity = (rawVelocity * octaveGainReduction) * rankContrast;

            // We still clamp safely, but the math guarantees it will never hard-clip
            dataPointer[startIndex + i] = static_cast<double>(std::min(renderVelocity, 1.0f));
        }
    }

    advanceBubbles();
    repaint();
}

void LineViz::paintBins(juce::Graphics& graphics) const {
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
    paintBins(graphics);
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

    const float height = static_cast<float>(getHeight());
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
        const juce::Colour color = juce::Colours::black.interpolatedWith(baseColor, 0.5f);

        graphics.setColour(color);

        // Draw a strict 1px vertical line exactly at the calculated center
        graphics.drawLine(targetCenter, startY, targetCenter, endY, 1.0f);
    }
}

void LineViz::advanceBubbles() {
    // 1. Move existing bubbles up smoothly by 1 pixel per frame
    for (auto& bubble : bubbles) {
        constexpr float speed = 1.0f;
        bubble.y -= speed;
    }

    // 2. Clean up bubbles that float completely off the top of the screen
    std::erase_if(bubbles, [](const Bubble& b) { return b.y < 0.0f; });

    if (displayMagnitudes.empty()) return;

    const int totalBins = static_cast<int>(displayMagnitudes.size());
    const int binsPerOctave = totalBins / PitchengaAudioProcessor::numOctaves;

    const float width = static_cast<float>(getWidth());
    const float height = static_cast<float>(getHeight());

    if (width <= 0.0f || height <= 0.0f || totalBins <= 0) return;

    const float barWidth = width / static_cast<float>(totalBins);

    // 3. Spawn new bubbles
    for (int i = 0; i < totalBins; ++i) {
        const double magnitude = displayMagnitudes[static_cast<size_t>(i)];

        if (magnitude > bubbleThreshold) {
            const float normalizedMagnitude = static_cast<float>(std::min(1.0, std::max(0.0, magnitude)));
            const float barHeight = normalizedMagnitude * height;

            // Spawn exactly at the tip of the current signal bar
            const float yPos = height - barHeight;

            const float chroma = static_cast<float>(i % binsPerOctave) * 12.0f / static_cast<float>(binsPerOctave);
            const juce::Colour color = ColorPalette::getContinuousColor(chroma);

            bubbles.push_back({static_cast<float>(i) * barWidth, yPos, barWidth, color});
        }
    }
}

void LineViz::paintBubbles(juce::Graphics& graphics) const {
    const float height = static_cast<float>(getHeight());

    // Calculate the Y coordinate that represents the bottom of the top 60%
    const float limitY = height * 1 - bubbleThreshold;

    for (const auto& b : bubbles) {
        if (b.y <= limitY) {
            const juce::Colour dimmedColor = juce::Colours::black.interpolatedWith(b.color, 0.6f);
            graphics.setColour(dimmedColor);

            // Draw a 1px tall rect. Because it spawns every frame, it forms a seamless streak
            graphics.fillRect(b.x, b.y, b.width, 1.0f);
        }
    }
}
