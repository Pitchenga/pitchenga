#include "CircleViz.h"
#include <cmath>
#include <algorithm>

CircleViz::CircleViz() {
    smoothedOctaveBins.resize(totalFoldedBins, 0.0);
}

void CircleViz::updateResults(const std::vector<double>& results) {
    if (results.size() == smoothedOctaveBins.size()) {
        std::ranges::copy(results, smoothedOctaveBins.begin());
        repaint();
    }
}

juce::Colour CircleViz::calculateColor(const float velocity, const float toneRatio) {
    float wrappedRatio = toneRatio;
    while (wrappedRatio < 0.0f) wrappedRatio += 12.0f;
    while (wrappedRatio >= 12.0f) wrappedRatio -= 12.0f;

    // Use std::round to snap to the NEAREST pitch center (C, C#, etc.)
    const float nearestPitch = std::round(wrappedRatio);

    // Diff will now perfectly range from -0.5 to +0.5 symmetrically
    const float diff = wrappedRatio - nearestPitch;

    int currentIdx = static_cast<int>(nearestPitch);
    if (currentIdx >= 12) currentIdx = 0; // Wrap B (11.6) -> 12 -> 0 (C)

    const juce::Colour toneColor = ColorPalette::chromaticScale[static_cast<size_t>(currentIdx)].color;

    juce::Colour guessColor;
    if (std::abs(diff) < 1e-5f) {
        guessColor = toneColor;
    } else {
        // Find the adjacent pitch we are bleeding into
        int pitchyIdx = diff < 0 ? currentIdx - 1 : currentIdx + 1;
        if (pitchyIdx < 0) pitchyIdx = 11;
        else if (pitchyIdx > 11) pitchyIdx = 0;

        const juce::Colour pitchyColor = ColorPalette::chromaticScale[static_cast<size_t>(pitchyIdx)].color;

        // Multiply by 2.0 so a 0.5 distance equals a 100% color blend
        const float interpolationWeight = std::abs(diff) * 2.0f;
        guessColor = toneColor.interpolatedWith(pitchyColor, interpolationWeight);
    }

    float colorVelocity = 0.3f + velocity * 1.2f;
    if (colorVelocity > 1.0f) colorVelocity = 1.0f;

    return juce::Colours::black.interpolatedWith(guessColor, colorVelocity);
}

void CircleViz::paint(juce::Graphics& g) {
    if (cachedFrame.isValid()) {
        g.drawImageAt(cachedFrame, 0, 0);
    } else {
        paintFrame(g);
    }

    const auto bounds = getLocalBounds().toFloat();
    const auto center = bounds.getCentre();
    const auto baseRadius = std::min(bounds.getWidth(), bounds.getHeight()) * 0.45f;

    // 1. Sort to get bin ranks (Matching Java indexToVelocityPairs)
    struct BinData {
        int index;
        float velocity;
    };
    std::vector<BinData> sortedBins(totalFoldedBins);
    for (int i = 0; i < totalFoldedBins; ++i) {
        sortedBins[static_cast<size_t>(i)] = {i, static_cast<float>(smoothedOctaveBins[static_cast<size_t>(i)])};
    }

    std::ranges::sort(
        sortedBins,
        [](const BinData& a, const BinData& b) {
            return a.velocity < b.velocity;
        }
    );

    std::vector<int> binOrders(totalFoldedBins);
    for (int rank = 0; rank < totalFoldedBins; ++rank) {
        binOrders[static_cast<size_t>(sortedBins[static_cast<size_t>(rank)].index)] = rank;
    }

    const int biggestBinNumber = sortedBins.back().index;

    // 2. Render Loop
    for (int i = 0; i < totalFoldedBins; ++i) {
        const auto rawVelocity = static_cast<float>(smoothedOctaveBins[static_cast<size_t>(i)]);

        // Rank-based amplification
        float renderVelocity = rawVelocity * (static_cast<float>(binOrders[static_cast<size_t>(i)]) * 0.011f);
        if (i == biggestBinNumber) {
            renderVelocity *= 1.3f;
        }
        renderVelocity = std::min(renderVelocity, 1.15f);

        const float toneRatio = static_cast<float>(i) / static_cast<float>(binsPerSemitone);
        const juce::Colour color = calculateColor(renderVelocity, toneRatio);

        // Use the amplified velocity for radius
        const float currentRadius = baseRadius * renderVelocity;

        auto& originalPath = segmentPaths[static_cast<size_t>(i)];
        auto transform = juce::AffineTransform::scale(currentRadius, currentRadius).translated(center.x, center.y);

        g.setColour(color);
        g.fillPath(originalPath, transform);

        g.strokePath(originalPath, strokeType, transform);
    }
}

void CircleViz::paintLabel(
    juce::Graphics& graphics,
    const juce::Point<float> center,
    const float baseRadius,
    const float startRadius,
    const int i,
    const float sin,
    const float cos
) {
    const float rLabel = baseRadius * startRadius;
    const float initialX = center.x + rLabel * cos;
    const float initialY = center.y + rLabel * sin;
    const juce::String name = ColorPalette::chromaticScale[static_cast<size_t>(i)].toneName;

    graphics.setFont(juce::FontOptions(baseRadius * 0.15f).withStyle("Bold"));
    const auto labelColor = calculateColor(0.1f, static_cast<float>(i));
    graphics.setColour(labelColor);

    juce::GlyphArrangement arrangement;
    arrangement.addLineOfText(graphics.getCurrentFont(), name, 0.0f, 0.0f);
    arrangement.justifyGlyphs(
        0,
        arrangement.getNumGlyphs(),
        initialX,
        initialY,
        0.0f,
        0.0f,
        juce::Justification::centred
    );

    const auto textBounds = arrangement.getBoundingBox(0, arrangement.getNumGlyphs(), true);
    const float ellipseHeight = textBounds.getWidth() * 0.5f + baseRadius * 0.03f;
    const float ellipseWidth =  textBounds.getHeight() * 0.5f + baseRadius * 0.03f;
    graphics.setColour(juce::Colours::black);
    graphics.fillEllipse(
        textBounds.getCentreX() - ellipseHeight,
        textBounds.getCentreY() - ellipseWidth,
        ellipseHeight * 2.0f,
        ellipseWidth * 2.0f
    );

    graphics.setColour(labelColor);
    arrangement.draw(graphics);
}

void CircleViz::paintFrame(juce::Graphics& graphics) const {
    const auto bounds = getLocalBounds().toFloat();
    const auto center = bounds.getCentre();
    const auto outerRadius = std::min(bounds.getWidth(), bounds.getHeight()) / 2.0f;

    constexpr float angleStep = juce::MathConstants<float>::twoPi / 12.0f;
    constexpr float startAngle = -juce::MathConstants<float>::halfPi;

    for (int i = 0; i < 12; ++i) {
        const float angle = startAngle + static_cast<float>(i) * angleStep;
        const float sin = std::sin(angle);
        const float cos = std::cos(angle);

        const auto color = ColorPalette::chromaticScale[static_cast<size_t>(i)].color;
        graphics.setColour(color);

        graphics.drawLine(
            center.x,
            center.y,
            center.x + outerRadius * cos,
            center.y + outerRadius * sin,
            2.0f
        );

        constexpr float labelStartRadius = 0.83f;
        paintLabel(graphics, center, outerRadius, labelStartRadius, i, sin, cos);
    }
}

void CircleViz::paintFrame() {
    const int width = getWidth();
    const int height = getHeight();
    if (width <= 0 || height <= 0) return;

    cachedFrame = juce::Image(juce::Image::ARGB, width, height, true);
    juce::Graphics graphics(cachedFrame);
    paintFrame(graphics);
}

void CircleViz::paintBins() {
    constexpr float angleStep = juce::MathConstants<float>::twoPi / static_cast<float>(totalFoldedBins);
    constexpr float rotation = 0.0f - 0.5f * angleStep;

    for (int i = 0; i < totalFoldedBins; ++i) {
        const float startAngle = static_cast<float>(i) * angleStep + rotation;

        // Add overlap to kill the anti-aliased gap
        const float endAngle = static_cast<float>(i + 1) * angleStep + rotation + 0.01f;

        juce::Path path;
        path.addCentredArc(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, startAngle, endAngle, true);
        path.lineTo(0.0f, 0.0f);
        path.closeSubPath();

        segmentPaths[static_cast<size_t>(i)] = path;
    }
}

void CircleViz::resized() {
    paintFrame();
    paintBins();
}
