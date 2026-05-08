#include "Eye.h"
#include <cmath>
#include <algorithm>
#include "../PluginProcessor.h"
#include "../Tone.h"

Eye::Eye(PitchengaAudioProcessor& processor) : processor(processor) {
    smoothedOctaveBins.resize(totalFoldedBins, 0.0);
}

void Eye::updateResults(const std::vector<double>& results) {
    if (results.size() == smoothedOctaveBins.size()) {
        std::ranges::copy(results, smoothedOctaveBins.begin());
        repaint();
    }
}

juce::Colour Eye::calculateColor(const float velocity, const float toneRatio) {
    const juce::Colour continuousColor = Tone::getContinuousColor(toneRatio);

    float colorVelocity = 0.3f + velocity * 1.2f;
    if (colorVelocity > 1.0f) colorVelocity = 1.0f;

    return juce::Colours::black.interpolatedWith(continuousColor, colorVelocity);
}

void Eye::paintBins(juce::Graphics& graphics) const {
    const auto bounds = getLocalBounds().toFloat();
    const auto center = bounds.getCentre();
    const auto outerRadius = std::min(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    const auto baseRadius = outerRadius / 1.15f;

    // Sort to get bin ranks (Matching Java indexToVelocityPairs)
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

    // Render Loop
    for (int i = 0; i < totalFoldedBins; ++i) {
        const auto rawVelocity = static_cast<float>(smoothedOctaveBins[static_cast<size_t>(i)]);

        // Rank-based amplification
        float renderVelocity = rawVelocity * (static_cast<float>(binOrders[static_cast<size_t>(i)]) * 0.011f);
        if (i == biggestBinNumber) {
            renderVelocity *= 1.3f;
        }
        renderVelocity = std::min(renderVelocity, 1.15f);
        if (paintLogo) {
            renderVelocity = 1.15f;
        }

        const float toneRatio = static_cast<float>(i) / static_cast<float>(binsPerSemitone);
        const juce::Colour color = calculateColor(renderVelocity, toneRatio);

        // Use the amplified velocity for radius
        const float currentRadius = baseRadius * renderVelocity;

        auto& originalPath = segmentPaths[static_cast<size_t>(i)];
        auto transform = juce::AffineTransform::scale(currentRadius, currentRadius).translated(center.x, center.y);

        graphics.setColour(color);
        graphics.fillPath(originalPath, transform);

        graphics.strokePath(originalPath, strokeType, transform);
    }
}

void Eye::paint(juce::Graphics& g) {
    if (cachedFrame.isValid()) {
        g.drawImageAt(cachedFrame, 0, 0);
    } else {
        buildFrame(g);
    }

    paintBins(g);
}

void Eye::paintLabel(
    juce::Graphics& graphics,
    const juce::Point<float> center,
    const float baseRadius,
    const float startRadius,
    const int i,
    const float sin,
    const float cos
) const {
    const float rLabel = baseRadius * startRadius;
    const float initialX = center.x + rLabel * cos;
    const float initialY = center.y + rLabel * sin;

    graphics.setFont(juce::FontOptions(baseRadius * 0.15f).withStyle("Bold"));
    const auto labelColor = calculateColor(0.1f, static_cast<float>(i));
    graphics.setColour(labelColor);

    const juce::String name = Tone::getToneName(i, processor.settings.isLetterNotation);
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

void Eye::buildFrame(juce::Graphics& graphics) const {
    const auto bounds = getLocalBounds().toFloat();
    const auto center = bounds.getCentre();
    const auto outerRadius = std::min(bounds.getWidth(), bounds.getHeight()) / 2.0f;

    constexpr float angleStep = juce::MathConstants<float>::twoPi / 12.0f;
    constexpr float startAngle = -juce::MathConstants<float>::halfPi;

    constexpr int numSegments = 360;
    constexpr float segmentAngleStep = juce::MathConstants<float>::twoPi / static_cast<float>(numSegments);

    for (int i = 0; i < numSegments; ++i) {
        const float arcStart = static_cast<float>(i) * segmentAngleStep;
        // Overlap slightly to prevent antialiasing gaps
        const float arcEnd = static_cast<float>(i + 1) * segmentAngleStep + 0.01f;
        const float chroma = static_cast<float>(i) / static_cast<float>(numSegments) * 12.0f;

        juce::Path arcPath;
        arcPath.addCentredArc(center.x, center.y, outerRadius - 1.0f, outerRadius - 1.0f, 0.0f, arcStart, arcEnd, true);

        graphics.setColour(Tone::getContinuousColor(chroma));
        graphics.strokePath(arcPath, juce::PathStrokeType(2.0f));
    }

    for (int i = 0; i < 12; ++i) {
        const float angle = startAngle + static_cast<float>(i) * angleStep;
        const float sin = std::sin(angle);
        const float cos = std::cos(angle);

        const auto color = Tone::chromaticScale[static_cast<size_t>(i)].color;
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

void Eye::paintFrame() {
    const int width = getWidth();
    const int height = getHeight();
    if (width <= 0 || height <= 0) return;

    cachedFrame = juce::Image(juce::Image::ARGB, width, height, true);
    juce::Graphics graphics(cachedFrame);
    buildFrame(graphics);
}

void Eye::buildBins() {
    constexpr float angleStep = juce::MathConstants<float>::twoPi / static_cast<float>(totalFoldedBins);
    constexpr float rotation = 0.0f - 0.5f * angleStep;

    for (int i = 0; i < totalFoldedBins; ++i) {
        const float startAngle = static_cast<float>(i) * angleStep + rotation;

        // Overlap to kill the anti-aliased gap
        const float endAngle = static_cast<float>(i + 1) * angleStep + rotation + 0.01f;

        juce::Path path;
        path.addCentredArc(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, startAngle, endAngle, true);
        path.lineTo(0.0f, 0.0f);
        path.closeSubPath();

        segmentPaths[static_cast<size_t>(i)] = path;
    }
}

void Eye::resized() {
    paintFrame();
    buildBins();
}
