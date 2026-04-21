#include "Tuna.h"
#include <cmath>

Tuna::Tuna() {
    // Pre-calculate the sine wave intensities once during construction
    for (int i = 0; i < strobeCycleWidth; ++i) {
        const float sineVal = std::sin(juce::MathConstants<float>::twoPi * static_cast<float>(i) / static_cast<float>(strobeCycleWidth));
        const float wave = 0.5f * (1.0f + sineVal); // 0.0 to 1.0

        // Intensity interpolates from 0.5 to 1.0
        strobeIntensities[static_cast<size_t>(i)] = 0.5f + (0.5f * wave);
    }
}

void Tuna::setPitchFrequency(const float frequencyHz) {
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

void Tuna::setRange(const float minMidiNote, const float maxMidiNote) {
    minMidi = minMidiNote;
    maxMidi = maxMidiNote;
    updateCachedLabels();
    repaint();
}

float Tuna::freqToMidi(const float freq) {
    if (freq <= 0.0f) return -1.0f;
    return 69.0f + 12.0f * std::log2(freq / 440.0f);
}

juce::String Tuna::getNoteName(const int midiNote) {
    int chroma = midiNote % 12;
    if (chroma < 0) chroma += 12;
    const int octave = midiNote / 12 - 1;
    return Tone::chromaticScale[static_cast<size_t>(chroma)].toneName + juce::String(octave);
}

void Tuna::paintLabel(juce::Graphics& graphics, const int midiNote, const float x, const float stripY) {
    const juce::String name = getNoteName(midiNote);
    graphics.saveState();

    const float labelWidth = getLabelWidth();
    const float labelHeight = getLabelHeight();

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

juce::Font Tuna::getLabelFont() {
    return {
        juce::FontOptions(tunaFontSize)
        .withStyle(tunaFontStyle)
        .withName(juce::Font::getDefaultMonospacedFontName())
    };
}

float Tuna::getLabelHeight() {
    return juce::GlyphArrangement::getStringWidth(getLabelFont(), "Ww8");
}

float Tuna::getLabelWidth() {
    return getLabelFont().getHeight();
}

float Tuna::getPreferredHeight() {
    return stripHeight + tickHeight + getLabelHeight();
}

void Tuna::updateCachedLabels() {
    const int width = getWidth();
    const int height = getHeight();
    if (width <= 0 || height <= 0) return;

    // Create a 2D image purely for the static text and ticks
    cachedLabels = juce::Image(juce::Image::ARGB, width, height, true);
    juce::Graphics graphics(cachedLabels);

    const float stripY = static_cast<float>(height) - stripHeight;
    graphics.setFont(getLabelFont());

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

void Tuna::resized() {
    updateCachedLabels();
}

void Tuna::paint(juce::Graphics& graphics) {
    const auto bounds = getLocalBounds().toFloat();
    const auto height = static_cast<float>(getHeight());
    const float stripY = height - stripHeight;
    const int width = getWidth();

    // Draw the live strobe gradient natively
    for (int x = 0; x < width; ++x) {
        const float horizontalFraction = width > 1 ? static_cast<float>(x) / static_cast<float>(width - 1) : 0.0f;
        const float midiAtX = minMidi + horizontalFraction * (maxMidi - minMidi);

        float chroma = std::fmod(midiAtX, 12.0f);
        if (chroma < 0.0f) chroma += 12.0f;

        juce::Colour color = Tone::getContinuousColor(chroma);

        // Sub-pixel continuous phase calculation prevents micro-stuttering and halting at slow speeds
        const float exactPhase = static_cast<float>(x) - strobePhase;
        float phaseF = std::fmod(exactPhase, static_cast<float>(strobeCycleWidth));
        if (phaseF < 0.0f) phaseF += static_cast<float>(strobeCycleWidth);

        const int index0 = static_cast<int>(phaseF);
        const int index1 = (index0 + 1) % strobeCycleWidth;
        const float frac = phaseF - static_cast<float>(index0);

        // Modulate color intensity with black to form the exact strobe effect using linear interpolation
        const float intensity = strobeIntensities[static_cast<size_t>(index0)] * (1.0f - frac) +
                                strobeIntensities[static_cast<size_t>(index1)] * frac;

        color = color.interpolatedWith(juce::Colours::black, 1.0f - intensity);

        graphics.setColour(color);
        graphics.fillRect(static_cast<float>(x), stripY, 1.0f, stripHeight);
    }

    // Overlay the pre-baked labels and ticks
    if (cachedLabels.isValid()) {
        graphics.drawImageAt(cachedLabels, 0, 0);
    }

    // Dynamic Illumination Overlay
    if (currentMidi >= minMidi && currentMidi <= maxMidi) {
        graphics.setFont(getLabelFont());

        const float labelStripY = height - stripHeight;

        // Find the nearest perfect whole note to illuminate
        const int nearestNote = static_cast<int>(std::round(currentMidi));
        const int startMidi = static_cast<int>(std::ceil(minMidi));
        const int endMidi = static_cast<int>(std::floor(maxMidi));

        // Light up the label
        if (nearestNote > startMidi && nearestNote < endMidi) {
            const float closestX = bounds.getWidth() * ((static_cast<float>(nearestNote) - minMidi) / (maxMidi - minMidi));

            int nearestChroma = nearestNote % 12;
            if (nearestChroma < 0) nearestChroma += 12;
            const juce::Colour toneColor = Tone::chromaticScale[static_cast<size_t>(nearestChroma)].color;
            graphics.setColour(toneColor);
            paintLabel(graphics, nearestNote, closestX, labelStripY);
        }

        // Draw the tuna needle
        const float pitchX = bounds.getWidth() * ((currentMidi - minMidi) / (maxMidi - minMidi));
        const float tunaStripY = height - stripHeight + tickHeight;

        juce::Path triangle;
        triangle.addTriangle(
            pitchX,
            tunaStripY,
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
}