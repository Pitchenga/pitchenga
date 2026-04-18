#include "CircleVisualizer.h"
#include <cmath>
#include <algorithm>

CircleVisualizer::CircleVisualizer()
{
    smoothedOctaveBins.resize (totalFoldedBins, 0.0);
}

void CircleVisualizer::updateResults (const std::vector<double>& results)
{
    if (results.size() == smoothedOctaveBins.size())
    {
        std::ranges::copy (results, smoothedOctaveBins.begin());
        repaint();
    }
}

juce::Colour CircleVisualizer::calculateColor (float velocity, float toneRatio)
{
    // 1. NO std::fmod() NEEDED.
    // toneRatio only ever ranges from -0.444 to 11.444.
    // A single addition handles the negative wrap perfectly.
    float wrappedRatio = toneRatio;
    if (wrappedRatio < 0.0f) wrappedRatio += 12.0f;

    int toneNumber = static_cast<int> (std::floor (wrappedRatio));
    float diff = wrappedRatio - static_cast<float> (toneNumber);

    // 2. NO MODULO NEEDED.
    // wrappedRatio is strictly [0.0, 12.0), so toneNumber is strictly 0 to 11.
    int currentIdx = toneNumber;
    juce::Colour toneColor = ColorPalette::chromaticScale[static_cast<size_t>(currentIdx)].color;

    // --- Port of getGuessAndPitchinessColor & transposePitch ---
    juce::Colour guessColor;
    if (std::abs(diff) < 1e-5f)
    {
        guessColor = toneColor;
    }
    else
    {
        // Transpose -1 or +1 step depending on diff direction
        int pitchyIdx = diff < 0 ? currentIdx - 1 : currentIdx + 1;
        if (pitchyIdx < 0) pitchyIdx += 12;
        pitchyIdx %= 12;

        // 3. NO MODULO NEEDED.
        // It only moves by exactly 1 step, so a simple bounds check wraps it perfectly.
        if (pitchyIdx < 0) pitchyIdx = 11;
        else if (pitchyIdx > 11) pitchyIdx = 0;

        juce::Colour pitchyColor = ColorPalette::chromaticScale[static_cast<size_t>(pitchyIdx)].color;

        // Simple approximation of the ordinal pitchinessDiff logic
        float pitchinessDiff = std::abs(diff);
        guessColor = toneColor.interpolatedWith (pitchyColor, pitchinessDiff);
    }

    float colorVelocity = 0.3f + velocity * 1.2f;
    if (colorVelocity > 1.0f) colorVelocity = 1.0f;

    return juce::Colours::black.interpolatedWith (guessColor, colorVelocity);
}

void CircleVisualizer::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    auto center = bounds.getCentre();
    auto baseRadius = std::min (bounds.getWidth(), bounds.getHeight()) * 0.45f;

    // 1. Sort to get bin ranks (Matching Java indexToVelocityPairs)
    struct BinData { int index; float velocity; };
    std::vector<BinData> sortedBins (totalFoldedBins);
    for (int i = 0; i < totalFoldedBins; ++i) {
        sortedBins[static_cast<size_t>(i)] = { i, static_cast<float>(smoothedOctaveBins[static_cast<size_t>(i)]) };
    }

    std::ranges::sort (sortedBins, [](const BinData& a, const BinData& b) {
        return a.velocity < b.velocity;
    });

    std::vector<int> binOrders (totalFoldedBins);
    for (int rank = 0; rank < totalFoldedBins; ++rank) {
        binOrders[static_cast<size_t>(sortedBins[static_cast<size_t>(rank)].index)] = rank;
    }

    int biggestBinNumber = sortedBins.back().index;

    // 2. Render Loop
    for (int i = 0; i < totalFoldedBins; ++i)
    {
        auto rawVelocity = static_cast<float>(smoothedOctaveBins[static_cast<size_t>(i)]);

        // Rank-based amplification
        float renderVelocity = rawVelocity * (static_cast<float>(binOrders[static_cast<size_t>(i)]) * 0.011f);
        if (i == biggestBinNumber) {
            renderVelocity *= 1.3f;
        }
        renderVelocity = std::min (renderVelocity, 1.15f);

        float toneRatio = static_cast<float> (i) / static_cast<float> (binsPerSemitone);
        juce::Colour color = calculateColor (renderVelocity, toneRatio);

        // Use the amplified velocity for radius
        float currentRadius = baseRadius * renderVelocity;

        auto& originalPath = segmentPaths[static_cast<size_t>(i)];
        auto transform = juce::AffineTransform::scale (currentRadius, currentRadius).translated (center.x, center.y);

        g.setColour (color);
        g.fillPath (originalPath, transform);

        g.strokePath (originalPath, strokeType, transform);
    }
}

void CircleVisualizer::resized()
{
    constexpr float angleStep = juce::MathConstants<float>::twoPi / static_cast<float>(totalFoldedBins);
    constexpr float rotation = 0.0f - 0.5f * angleStep;

    for (int i = 0; i < totalFoldedBins; ++i)
    {
        const float startAngle = static_cast<float>(i) * angleStep + rotation;
        const float endAngle = static_cast<float>(i + 1) * angleStep + rotation;

        juce::Path p;
        p.addCentredArc (0.0f, 0.0f, 1.0f, 1.0f, 0.0f, startAngle, endAngle, true);
        p.lineTo (0.0f, 0.0f);
        p.closeSubPath();

        segmentPaths[static_cast<size_t>(i)] = p;
    }
}
