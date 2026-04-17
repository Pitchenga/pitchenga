#include "LineTuner.h"
#include <cmath>

LineTuner::LineTuner()
{
    setSize (800, 80); 
}

void LineTuner::setPitchFrequency (float frequencyHz)
{
    if (frequencyHz > 0.0f) {
        currentMidi = freqToMidi (frequencyHz);
    } else {
        currentMidi = -1.0f;
    }
    repaint();
}

void LineTuner::setRange (float minMidiNote, float maxMidiNote)
{
    minMidi = minMidiNote;
    maxMidi = maxMidiNote;
    updateCachedGradient();
    repaint();
}

float LineTuner::freqToMidi (float freq) const
{
    if (freq <= 0.0f) return -1.0f;
    return 69.0f + 12.0f * std::log2 (freq / 440.0f);
}

juce::String LineTuner::getNoteName (int midiNote) const
{
    int chroma = midiNote % 12;
    if (chroma < 0) chroma += 12;
    int octave = (midiNote / 12) - 1;
    return ColorPalette::chromaticScale[static_cast<size_t>(chroma)].toneName + juce::String(octave);
}

juce::Font LineTuner::getTunerFont() const
{
    return juce::Font (juce::FontOptions (tunerFontSize).withStyle (tunerFontStyle));
}

void LineTuner::updateCachedGradient()
{
    int width = getWidth();
    int height = getHeight();
    if (width <= 0 || height <= 0) return;

    // Create a 2D image so we can bake the text and ticks directly into it
    cachedGradient = juce::Image (juce::Image::ARGB, width, height, true);

    int stripHeight = 16;
    int stripY = height - stripHeight;

    // 1. Bake the continuous gradient
    juce::Image::BitmapData data (cachedGradient, juce::Image::BitmapData::writeOnly);
    for (int x = 0; x < width; ++x)
    {
        float horizontalFraction = width > 1 ? static_cast<float>(x) / static_cast<float>(width - 1) : 0.0f;
        float midiAtX = minMidi + horizontalFraction * (maxMidi - minMidi);

        float chroma = std::fmod (midiAtX, 12.0f);
        if (chroma < 0.0f) chroma += 12.0f;

        juce::Colour fullColor = ColorPalette::getContinuousColor (chroma);

        for (int y = stripY; y < height; ++y) {
            data.setPixelColour (x, y, fullColor);
        }
    }

    // 2. Bake the 30% dimmed labels and 1x5 ticks into the image
    juce::Graphics graphics (cachedGradient);
    graphics.setFont (getTunerFont());

    int startMidi = static_cast<int>(std::ceil(minMidi));
    int endMidi = static_cast<int>(std::floor(maxMidi));

    for (int note = startMidi; note <= endMidi; ++note)
    {
        float x = static_cast<float>(width) * ((static_cast<float>(note) - minMidi) / (maxMidi - minMidi));

        int chroma = note % 12;
        if (chroma < 0) chroma += 12;

        juce::Colour fullColor = ColorPalette::chromaticScale[static_cast<size_t>(chroma)].color;
        juce::Colour dimmedColor = juce::Colours::black.interpolatedWith (fullColor, dimmingFactor);

        graphics.setColour (dimmedColor);

        // Label
        juce::String name = getNoteName(note);
        graphics.saveState();
        graphics.addTransform (juce::AffineTransform::rotation (-juce::MathConstants<float>::halfPi, x, static_cast<float>(stripY - 5)));
        graphics.drawText (name, static_cast<int>(x), stripY - 5 - 15, 50, 30, juce::Justification::centredLeft);
        graphics.restoreState();

        // 1x5 tick
        graphics.setColour (juce::Colours::black);
        graphics.fillRect (x - 0.5f, static_cast<float>(stripY), 1.0f, 5.0f);

    }
}

void LineTuner::resized()
{
    updateCachedGradient();
}

void LineTuner::paint (juce::Graphics& graphics)
{
    auto bounds = getLocalBounds().toFloat();
    int height = getHeight();

    // 1. Draw the fully baked static background (Opaque overwrite, zero alpha tax)
    if (cachedGradient.isValid()) {
        graphics.drawImageAt (cachedGradient, 0, 0);
    }

    // 2. Dynamic Illumination Overlay
    if (currentMidi >= minMidi && currentMidi <= maxMidi)
    {
        graphics.setFont (getTunerFont());

        int stripHeight = 16;
        int stripY = height - stripHeight;

        // Find the nearest perfect whole note to illuminate
        int nearestNote = static_cast<int>(std::round(currentMidi));

        // Light up the label
        if (static_cast<float>(nearestNote) >= std::ceil(minMidi) && static_cast<float>(nearestNote) <= std::floor(maxMidi))
        {
            float closestX = bounds.getWidth() * ((static_cast<float>(nearestNote) - minMidi) / (maxMidi - minMidi));
            
            int nearestChroma = nearestNote % 12;
            if (nearestChroma < 0) nearestChroma += 12;

            juce::Colour toneColor = ColorPalette::chromaticScale[static_cast<size_t>(nearestChroma)].color;
            graphics.setColour (toneColor);
            juce::String name = getNoteName(nearestNote);
            
            graphics.saveState();
            graphics.addTransform (juce::AffineTransform::rotation (-juce::MathConstants<float>::halfPi, closestX, static_cast<float>(stripY - 5)));
            graphics.drawText (name, static_cast<int>(closestX), stripY - 5 - 15, 50, 30, juce::Justification::centredLeft);
            graphics.restoreState();
        }

        // Draw the exact float pitch indicator line at 100% brightness
        float pitchX = bounds.getWidth() * ((currentMidi - minMidi) / (maxMidi - minMidi));
        
        float exactChroma = std::fmod (currentMidi, 12.0f);
        if (exactChroma < 0.0f) exactChroma += 12.0f;
        
        graphics.setColour (juce::Colours::black);
        graphics.drawLine (pitchX, static_cast<float>(stripY), pitchX, static_cast<float>(height), 1.0f);
    }
}