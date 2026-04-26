#include "Needle.h"
#include <cmath>
#include "../Tone.h"
#include "../Common.h"

Needle::Needle() {
    // Pre-calculate the sine wave intensities once during construction
    for (int i = 0; i < strobeCycleWidth; ++i) {
        const float sineVal = std::sin(
            juce::MathConstants<float>::twoPi * static_cast<float>(i) / static_cast<float>(strobeCycleWidth)
        );
        const float wave = 0.5f * (1.0f + sineVal); // 0.0 to 1.0

        // Intensity interpolates from 0.5 to 1.0
        strobeIntensities[static_cast<size_t>(i)] = 0.5f + (0.5f * wave);
    }
}

void Needle::setPitchFrequency(const float frequencyHz) {
    if (frequencyHz > 0.0f) {
        currentMidi = freqToMidi(frequencyHz);

        const float error = currentMidi - std::round(currentMidi);

        // Speed optimized to avoid the "Wagon-Wheel Effect" optical illusion while maintaining high visibility
        constexpr float maxPixelsPerFrame = 20.0f;
        targetVelocity = error * maxPixelsPerFrame;
        framesSinceSignalLost = 0;
    } else {
        // We intentionally do NOT reset currentMidi here so the needle remains visible at the last known pitch.
        framesSinceSignalLost++;
        if (framesSinceSignalLost > spinHoldFrames) {
            targetVelocity = 0.0f;
        }
    }

    // Mechanical Inertia: A physical strobe disc cannot instantly stop or change direction.
    // This exponential smoother cures both MPM frame drops and natural acoustic jitter.
    constexpr float inertiaFactor = 0.05f;
    currentVelocity = currentVelocity * (1.0f - inertiaFactor) + targetVelocity * inertiaFactor;

    strobePhase += currentVelocity;

    while (strobePhase >= static_cast<float>(strobeCycleWidth)) strobePhase -= static_cast<float>(strobeCycleWidth);
    while (strobePhase < 0.0f) strobePhase += static_cast<float>(strobeCycleWidth);

    repaint();
}

void Needle::setRange(const float minMidiNote, const float maxMidiNote) {
    minMidi = minMidiNote;
    maxMidi = maxMidiNote;
    buildFrame();
    repaint();
}

float Needle::freqToMidi(const float freq) {
    if (freq <= 0.0f) return -1.0f;
    return 69.0f + 12.0f * std::log2(freq / 440.0f);
}

juce::String Needle::getNoteName(const int midiNote) {
    int chroma = midiNote % 12;
    if (chroma < 0) chroma += 12;
    const int octave = midiNote / 12 - 1;
    return Tone::chromaticScale[static_cast<size_t>(chroma)].toneName + juce::String(octave);
}

void Needle::paintLabel(juce::Graphics& graphics, const int midiNote, const float x, const float stripY) {
    const juce::String name = getNoteName(midiNote);
    graphics.saveState();

    const float labelWidth = Common::getLabelWidth();
    const float labelHeight = Common::getLabelHeight();

    graphics.addTransform(juce::AffineTransform::rotation(-juce::MathConstants<float>::halfPi, x, stripY - tickHeight));

    // In the rotated context:
    // Local X goes UP physically. We start at x, so it extends UP from the tick by labelH (string length).
    // Local Y goes RIGHT physically. To center horizontally, we start at -labelW/2 relative to rotation origin Y.
    graphics.drawText(
        name,
        juce::Rectangle(
            x,
            stripY - tickHeight - labelWidth / 2.0f,
            labelHeight,
            labelWidth
        ),
        juce::Justification::centredLeft,
        false
    );
    graphics.restoreState();
}


float Needle::getPreferredHeight() {
    return stripHeight + tickHeight + Common::getLabelHeight();
}

void Needle::buildFrame() {
    const int width = getWidth();
    const int height = getHeight();
    if (width <= 0 || height <= 0) return;

    // --- Pre-render Background Gradient ---
    cachedGradient = juce::Image(juce::Image::RGB, width, static_cast<int>(stripHeight), true);
    juce::Graphics bgGraphics(cachedGradient);

    for (int x = 0; x < width; ++x) {
        const float horizontalFraction = width > 1 ? static_cast<float>(x) / static_cast<float>(width - 1) : 0.0f;
        const float midiAtX = minMidi + horizontalFraction * (maxMidi - minMidi);

        float chroma = std::fmod(midiAtX, 12.0f);
        if (chroma < 0.0f) chroma += 12.0f;

        bgGraphics.setColour(Tone::getContinuousColor(chroma, true));
        bgGraphics.drawVerticalLine(x, 0.0f, stripHeight);
    }

    // --- Pre-render Static Labels and Ticks ---
    cachedLabels = juce::Image(juce::Image::ARGB, width, height, true);
    juce::Graphics graphics(cachedLabels);

    const float stripY = static_cast<float>(height) - stripHeight;
    graphics.setFont(Common::getLabelFont());

    const int startMidi = static_cast<int>(std::ceil(minMidi));
    const int endMidi = static_cast<int>(std::floor(maxMidi));

    for (int note = startMidi; note <= endMidi; ++note) {
        const float x = static_cast<float>(width) * ((static_cast<float>(note) - minMidi) / (maxMidi - minMidi));

        int chroma = note % 12;
        if (chroma < 0) chroma += 12;

        const juce::Colour fullColor = Tone::chromaticScale[static_cast<size_t>(chroma)].color;
        graphics.setColour(fullColor);

        if (note > startMidi && note < endMidi) paintLabel(graphics, note, x, stripY);

        // Tick
        graphics.setColour(juce::Colours::black);
        graphics.fillRect(x - 0.5f, stripY, 1.0f, tickHeight);
    }
}

void Needle::resized() {
    buildFrame();
}

void Needle::paintStrobeOverlay(juce::Graphics& graphics, const float stripY, const int width) {
    if (currentMidi >= minMidi && currentMidi <= maxMidi) {
        constexpr float strobeSpreadMidi = 10.0f;

        // Calculate the pixel bounds of the strobe effect to avoid full-width iteration
        const float pitchX = static_cast<float>(width) * ((currentMidi - minMidi) / (maxMidi - minMidi));
        const float spreadPx = (strobeSpreadMidi / (maxMidi - minMidi)) * static_cast<float>(width);

        const int startX = std::max(0, static_cast<int>(std::floor(pitchX - spreadPx)));
        const int endX = std::min(width - 1, static_cast<int>(std::ceil(pitchX + spreadPx)));

        for (int x = startX; x <= endX; ++x) {
            const float horizontalFraction = width > 1 ? static_cast<float>(x) / static_cast<float>(width - 1) : 0.0f;
            const float midiAtX = minMidi + horizontalFraction * (maxMidi - minMidi);
            const float distanceMidi = std::abs(midiAtX - currentMidi);

            if (distanceMidi < strobeSpreadMidi) {
                const float exactPhase = static_cast<float>(x) - strobePhase;
                float phaseF = std::fmod(exactPhase, static_cast<float>(strobeCycleWidth));
                if (phaseF < 0.0f) phaseF += static_cast<float>(strobeCycleWidth);

                const int index0 = static_cast<int>(phaseF);
                const int index1 = (index0 + 1) % strobeCycleWidth;
                const float frac = phaseF - static_cast<float>(index0);

                const float intensity =
                    strobeIntensities[static_cast<size_t>(index0)] * (1.0f - frac)
                    + strobeIntensities[static_cast<size_t>(index1)] * frac;

                const float fadeFactor = 1.0f - (distanceMidi / strobeSpreadMidi);
                const float dimming = (1.0f - intensity) * fadeFactor;

                graphics.setColour(juce::Colours::black.withAlpha(dimming));
                graphics.drawVerticalLine(x, stripY, stripY + stripHeight);
            }
        }
    }
}

void Needle::paintTunerNeedle(juce::Graphics& graphics, const juce::Rectangle<float> bounds, const float height) const {
    const float pitchX = bounds.getWidth() * ((currentMidi - minMidi) / (maxMidi - minMidi));
    const float needleStripY = height - stripHeight + tickHeight;

    juce::Path triangle;
    triangle.addTriangle(
        pitchX,
        needleStripY,
        pitchX - needleTriangleWidth * 0.5f,
        height,
        pitchX + needleTriangleWidth * 0.5f,
        height
    );

    // Fill the needle with the exact continuous pitch color so it pops out of the darkness
    float exactChroma = std::fmod(currentMidi, 12.0f);
    if (exactChroma < 0.0f) exactChroma += 12.0f;
    const juce::Colour exactPitchColor = Tone::getContinuousColor(exactChroma);

    graphics.setColour(exactPitchColor);
    graphics.fillPath(triangle);

    graphics.setColour(juce::Colours::black);
    graphics.strokePath(triangle, juce::PathStrokeType(2.0f));
}

void Needle::paintLabelHighlight(juce::Graphics& graphics, const juce::Rectangle<float> bounds, const float height) const {
    graphics.setFont(Common::getLabelFont());

    const float labelStripY = height - stripHeight;

    // Find the nearest perfect whole note to illuminate
    const int nearestNote = static_cast<int>(std::round(currentMidi));
    const int startMidi = static_cast<int>(std::ceil(minMidi));
    const int endMidi = static_cast<int>(std::floor(maxMidi));

    // Light up the label
    if (nearestNote > startMidi && nearestNote < endMidi) {
        const float closestX = bounds.getWidth() * ((static_cast<float>(nearestNote) - minMidi) / (maxMidi -
            minMidi));

        int nearestChroma = nearestNote % 12;
        if (nearestChroma < 0) nearestChroma += 12;
        const juce::Colour toneColor = Tone::chromaticScale[static_cast<size_t>(nearestChroma)].color;
        graphics.setColour(toneColor);
        paintLabel(graphics, nearestNote, closestX, labelStripY);
    }
}

void Needle::paint(juce::Graphics& graphics) {
    if (!cachedGradient.isValid() || !cachedLabels.isValid()) {
        buildFrame();
    }

    const auto bounds = getLocalBounds().toFloat();
    const auto height = static_cast<float>(getHeight());
    const float stripY = height - stripHeight;
    const int width = getWidth();

    // Draw static background gradient
    if (cachedGradient.isValid()) {
        graphics.drawImageAt(cachedGradient, 0, static_cast<int>(stripY));
    }

    // Overlay dynamic strobe shadows
    paintStrobeOverlay(graphics, stripY, width);

    // Overlay the pre-baked labels and ticks
    if (cachedLabels.isValid()) {
        graphics.drawImageAt(cachedLabels, 0, 0);
    }

    if (currentMidi >= minMidi && currentMidi <= maxMidi) {
        paintLabelHighlight(graphics, bounds, height);
        paintTunerNeedle(graphics, bounds, height);
    }
}
