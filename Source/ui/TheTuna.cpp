#include "TheTuna.h"
#include <cmath>

TheTuna::TheTuna()
= default;

void TheTuna::setPitchFrequency(const float frequencyHz) {
    if (frequencyHz > 0.0f) {
        currentMidi = freqToMidi(frequencyHz);
    } else {
        currentMidi = -1.0f;
    }
    repaint();
}

void TheTuna::setRange(const float minMidiNote, const float maxMidiNote) {
    minMidi = minMidiNote;
    maxMidi = maxMidiNote;
    updateCachedGradient();
    repaint();
}

float TheTuna::freqToMidi(const float freq) {
    if (freq <= 0.0f) return -1.0f;
    return 69.0f + 12.0f * std::log2(freq / 440.0f);
}

juce::String TheTuna::getNoteName(const int midiNote) {
    int chroma = midiNote % 12;
    if (chroma < 0) chroma += 12;
    const int octave = midiNote / 12 - 1;
    return Palette::chromaticScale[static_cast<size_t>(chroma)].toneName + juce::String(octave);
}

void TheTuna::paintLabel(juce::Graphics& graphics, const int midiNote, const float x, const float stripY) {
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

juce::Font TheTuna::getLabelFont() {
    return {
        juce::FontOptions(tunaFontSize)
        .withStyle(tunaFontStyle)
        .withName(juce::Font::getDefaultMonospacedFontName())
    };
}

float TheTuna::getLabelHeight() {
    return juce::GlyphArrangement::getStringWidth(getLabelFont(), "Ww8");
}

float TheTuna::getLabelWidth() {
    return getLabelFont().getHeight();
}

float TheTuna::getPreferredHeight() {
    return stripHeight + tickHeight + getLabelHeight();
}

void TheTuna::updateCachedGradient() {
    const int width = getWidth();
    const int height = getHeight();
    if (width <= 0 || height <= 0) return;

    // Create a 2D image so we can bake the text and ticks directly into it
    cachedGradient = juce::Image(juce::Image::ARGB, width, height, true);

    const float stripY = static_cast<float>(height) - stripHeight;

    // Bake the continuous gradient
    const juce::Image::BitmapData data(cachedGradient, juce::Image::BitmapData::writeOnly);
    for (int x = 0; x < width; ++x) {
        const float horizontalFraction = width > 1 ? static_cast<float>(x) / static_cast<float>(width - 1) : 0.0f;
        const float midiAtX = minMidi + horizontalFraction * (maxMidi - minMidi);

        float chroma = std::fmod(midiAtX, 12.0f);
        if (chroma < 0.0f) chroma += 12.0f;

        const juce::Colour fullColor = Palette::getContinuousColor(chroma);

        for (int y = static_cast<int>(stripY); y < height; ++y) {
            data.setPixelColour(x, y, fullColor);
        }
    }

    // Bake the labels and ticks into the image
    juce::Graphics graphics(cachedGradient);
    graphics.setFont(getLabelFont());

    const int startMidi = static_cast<int>(std::ceil(minMidi));
    const int endMidi = static_cast<int>(std::floor(maxMidi));

    for (int note = startMidi; note <= endMidi; ++note) {
        const float x = static_cast<float>(width) * ((static_cast<float>(note) - minMidi) / (maxMidi - minMidi));

        int chroma = note % 12;
        if (chroma < 0) chroma += 12;

        const juce::Colour fullColor = Palette::chromaticScale[static_cast<size_t>(chroma)].color;
        graphics.setColour(fullColor);

        if (note > startMidi && note < endMidi) paintLabel(graphics, note, x, stripY);

        // Tick
        graphics.setColour(juce::Colours::black);
        graphics.fillRect(x - 0.5f, stripY, 1.0f, tickHeight);
    }
}

void TheTuna::resized() {
    updateCachedGradient();
}

void TheTuna::paint(juce::Graphics& graphics) {
    const auto bounds = getLocalBounds().toFloat();
    const auto height = static_cast<float>(getHeight());

    // Draw the fully baked static background (Opaque overwrite, zero alpha tax)
    if (cachedGradient.isValid()) {
        graphics.drawImageAt(cachedGradient, 0, 0);
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
            const juce::Colour toneColor = Palette::chromaticScale[static_cast<size_t>(nearestChroma)].color;
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

        graphics.setColour(juce::Colours::black);
        graphics.strokePath(triangle, juce::PathStrokeType(2.0f));
    }
}
