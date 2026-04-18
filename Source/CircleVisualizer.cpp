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
    float wrappedRatio = toneRatio;
    if (wrappedRatio < 0.0f) wrappedRatio += 12.0f;

    int toneNumber = static_cast<int> (std::floor (wrappedRatio));
    float diff = wrappedRatio - static_cast<float> (toneNumber);

    int currentIdx = toneNumber % 12;
    juce::Colour toneColor = ColorPalette::chromaticScale[static_cast<size_t>(currentIdx)].color;

    juce::Colour guessColor;
    if (std::abs(diff) < 1e-5f)
    {
        guessColor = toneColor;
    }
    else
    {
        int pitchyIdx = diff < 0 ? currentIdx - 1 : currentIdx + 1;
        if (pitchyIdx < 0) pitchyIdx += 12;
        pitchyIdx %= 12;

        juce::Colour pitchyColor = ColorPalette::chromaticScale[static_cast<size_t>(pitchyIdx)].color;

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

    for (int i = 0; i < totalFoldedBins; ++i)
    {
        auto rawVelocity = static_cast<float>(smoothedOctaveBins[static_cast<size_t>(i)]);

        float renderVelocity = rawVelocity * (static_cast<float>(binOrders[static_cast<size_t>(i)]) * 0.011f);
        if (i == biggestBinNumber) {
            renderVelocity *= 1.3f;
        }
        renderVelocity = std::min (renderVelocity, 1.15f);

        float toneRatio = static_cast<float> (i) / static_cast<float> (binsPerSemitone);
        juce::Colour color = calculateColor (renderVelocity, toneRatio);

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
