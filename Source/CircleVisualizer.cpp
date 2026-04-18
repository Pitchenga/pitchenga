#include "CircleVisualizer.h"
#include <cmath>
#include <algorithm>

CircleVisualizer::CircleVisualizer() {
    smoothedOctaveBins.resize(totalFoldedBins, 0.0);
}

void CircleVisualizer::updateResults(const std::vector<double>& results) {
    if (results.size() == smoothedOctaveBins.size()) {
        std::ranges::copy(results, smoothedOctaveBins.begin());
        repaint();
    }
}

juce::Colour CircleVisualizer::calculateColor(const float velocity, const float toneRatio) {
    // 1. NO std::fmod() NEEDED.
    // toneRatio only ever ranges from -0.444 to 11.444.
    // A single addition handles the negative wrap perfectly.
    float wrappedRatio = toneRatio;
    if (wrappedRatio < 0.0f) wrappedRatio += 12.0f;

    const int toneNumber = static_cast<int>(std::floor(wrappedRatio));
    const float diff = wrappedRatio - static_cast<float>(toneNumber);

    // 2. NO MODULO NEEDED.
    // wrappedRatio is strictly [0.0, 12.0), so toneNumber is strictly 0 to 11.
    const int currentIdx = toneNumber;
    const juce::Colour toneColor = ColorPalette::chromaticScale[static_cast<size_t>(currentIdx)].color;

    // --- Port of getGuessAndPitchinessColor & transposePitch ---
    juce::Colour guessColor;
    if (std::abs(diff) < 1e-5f) {
        guessColor = toneColor;
    } else {
        // Transpose -1 or +1 step depending on diff direction
        int pitchyIdx = diff < 0 ? currentIdx - 1 : currentIdx + 1;
        if (pitchyIdx < 0) pitchyIdx += 12;
        pitchyIdx %= 12;

        // 3. NO MODULO NEEDED.
        // It only moves by exactly 1 step, so a simple bounds check wraps it perfectly.
        if (pitchyIdx < 0) pitchyIdx = 11;
        else if (pitchyIdx > 11) pitchyIdx = 0;

        const juce::Colour pitchyColor = ColorPalette::chromaticScale[static_cast<size_t>(pitchyIdx)].color;

        // Simple approximation of the ordinal pitchinessDiff logic
        const float pitchinessDiff = std::abs(diff);
        guessColor = toneColor.interpolatedWith(pitchyColor, pitchinessDiff);
    }

    float colorVelocity = 0.3f + velocity * 1.2f;
    if (colorVelocity > 1.0f) colorVelocity = 1.0f;

    return juce::Colours::black.interpolatedWith(guessColor, colorVelocity);
}

void CircleVisualizer::paint(juce::Graphics& g) {
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

void CircleVisualizer::paintLabel(
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
    arrangement.draw(graphics);
}

void CircleVisualizer::paintFrame(juce::Graphics& graphics) const {
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

void CircleVisualizer::paintFrame() {
    const int width = getWidth();
    const int height = getHeight();
    if (width <= 0 || height <= 0) return;

    cachedFrame = juce::Image(juce::Image::ARGB, width, height, true);
    juce::Graphics graphics(cachedFrame);
    paintFrame(graphics);
}

void CircleVisualizer::paintBins() {
    constexpr float angleStep = juce::MathConstants<float>::twoPi / static_cast<float>(totalFoldedBins);
    constexpr float rotation = 0.0f - 0.5f * angleStep;

    for (int i = 0; i < totalFoldedBins; ++i) {
        const float startAngle = static_cast<float>(i) * angleStep + rotation;
        const float endAngle = static_cast<float>(i + 1) * angleStep + rotation;

        juce::Path path;
        path.addCentredArc(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, startAngle, endAngle, true);
        path.lineTo(0.0f, 0.0f);
        path.closeSubPath();

        segmentPaths[static_cast<size_t>(i)] = path;
    }
}

void CircleVisualizer::resized() {
    paintFrame();
    paintBins();
}
