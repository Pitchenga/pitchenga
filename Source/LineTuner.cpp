#include "LineTuner.h"
#include <cmath>

LineTuner::LineTuner()
= default;

void LineTuner::setPitchFrequency(float frequencyHz) {
    if (frequencyHz > 0.0f) {
        currentMidi = freqToMidi(frequencyHz);
    } else {
        currentMidi = -1.0f;
    }
    repaint();
}

void LineTuner::setRange(float minMidiNote, float maxMidiNote) {
    minMidi = minMidiNote;
    maxMidi = maxMidiNote;
    updateCachedGradient();
    repaint();
}

float LineTuner::freqToMidi(float freq) {
    if (freq <= 0.0f) return -1.0f;
    return 69.0f + 12.0f * std::log2(freq / 440.0f);
}

juce::String LineTuner::getNoteName(int midiNote) {
    int chroma = midiNote % 12;
    if (chroma < 0) chroma += 12;
    int octave = midiNote / 12 - 1;
    return ColorPalette::chromaticScale[static_cast<size_t>(chroma)].toneName + juce::String(octave);
}

void LineTuner::paintLabel(juce::Graphics& graphics, int midiNote, float x, float stripY) {
    juce::String name = getNoteName(midiNote);
    graphics.saveState();

    float labelWidth = getLabelWidth();
    float labelHeight = getLabelHeight();

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
        juce::Justification::centredLeft
    );
    graphics.restoreState();
}

juce::Font LineTuner::getLabelFont() {
    return {
        juce::FontOptions(tunerFontSize)
        .withStyle(tunerFontStyle)
        .withName(juce::Font::getDefaultMonospacedFontName())
    };
}

float LineTuner::getLabelHeight() {
    return getLabelFont().getStringWidthFloat("Ww8");
}

float LineTuner::getLabelWidth() {
    return getLabelFont().getHeight();
}

float LineTuner::getPreferredHeight() {
    return stripHeight + tickHeight + getLabelHeight();
}

void LineTuner::updateCachedGradient() {
    int width = getWidth();
    int height = getHeight();
    if (width <= 0 || height <= 0) return;

    // Create a 2D image so we can bake the text and ticks directly into it
    cachedGradient = juce::Image(juce::Image::ARGB, width, height, true);

    float stripY = static_cast<float>(height) - stripHeight;

    // 1. Bake the continuous gradient
    juce::Image::BitmapData data(cachedGradient, juce::Image::BitmapData::writeOnly);
    for (int x = 0; x < width; ++x) {
        float horizontalFraction = width > 1 ? static_cast<float>(x) / static_cast<float>(width - 1) : 0.0f;
        float midiAtX = minMidi + horizontalFraction * (maxMidi - minMidi);

        float chroma = std::fmod(midiAtX, 12.0f);
        if (chroma < 0.0f) chroma += 12.0f;

        juce::Colour fullColor = ColorPalette::getContinuousColor(chroma);

        for (int y = static_cast<int>(stripY); y < height; ++y) {
            data.setPixelColour(x, y, fullColor);
        }
    }

    // 2. Bake the labels and ticks into the image
    juce::Graphics graphics(cachedGradient);
    graphics.setFont(getLabelFont());

    int startMidi = static_cast<int>(std::ceil(minMidi));
    int endMidi = static_cast<int>(std::floor(maxMidi));

    for (int note = startMidi; note <= endMidi; ++note) {
        float x = static_cast<float>(width) * ((static_cast<float>(note) - minMidi) / (maxMidi - minMidi));

        int chroma = note % 12;
        if (chroma < 0) chroma += 12;

        juce::Colour fullColor = ColorPalette::chromaticScale[static_cast<size_t>(chroma)].color;
        juce::Colour dimmedColor = juce::Colours::black.interpolatedWith(fullColor, dimmingFactor);
        graphics.setColour(dimmedColor);

        if (note > startMidi && note < endMidi) paintLabel(graphics, note, x, stripY);

        // Tick
        graphics.setColour(juce::Colours::black);
        graphics.fillRect(x - 0.5f, stripY, 1.0f, tickHeight);
    }
}

void LineTuner::resized() {
    updateCachedGradient();
}

void LineTuner::paint(juce::Graphics& graphics) {
    auto bounds = getLocalBounds().toFloat();
    auto height = static_cast<float>(getHeight());

    // 1. Draw the fully baked static background (Opaque overwrite, zero alpha tax)
    if (cachedGradient.isValid()) {
        graphics.drawImageAt(cachedGradient, 0, 0);
    }

    // 2. Dynamic Illumination Overlay
    if (currentMidi >= minMidi && currentMidi <= maxMidi) {
        graphics.setFont(getLabelFont());

        float labelStripY = height - stripHeight;

        // Find the nearest perfect whole note to illuminate
        int nearestNote = static_cast<int>(std::round(currentMidi));
        int startMidi = static_cast<int>(std::ceil(minMidi));
        int endMidi = static_cast<int>(std::floor(maxMidi));

        // Light up the label
        if (nearestNote > startMidi && nearestNote < endMidi) {
            float closestX = bounds.getWidth() * ((static_cast<float>(nearestNote) - minMidi) / (maxMidi - minMidi));

            int nearestChroma = nearestNote % 12;
            if (nearestChroma < 0) nearestChroma += 12;
            juce::Colour toneColor = ColorPalette::chromaticScale[static_cast<size_t>(nearestChroma)].color;
            graphics.setColour(toneColor);
            paintLabel(graphics, nearestNote, closestX, labelStripY);
        }

        // Draw the tuner needle
        float pitchX = bounds.getWidth() * ((currentMidi - minMidi) / (maxMidi - minMidi));
        float tunerStripY = height - stripHeight + tickHeight;

        juce::Path triangle;
        triangle.addTriangle(
            pitchX,
            tunerStripY,
            pitchX - needleTriangleWidth * 0.5f,
            height,
            pitchX + needleTriangleWidth * 0.5f,
            height
        );

        graphics.setColour(juce::Colours::black);
        graphics.strokePath(triangle, juce::PathStrokeType(2.0f));
    }
}
