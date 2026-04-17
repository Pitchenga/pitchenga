#include "LineTuner.h"
#include <cmath>

LineTuner::LineTuner()
{
    setSize (800, 24); 
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
    return ColorPalette::chromaticScale[static_cast<size_t>(chroma)].name + juce::String(octave);
}

void LineTuner::updateCachedGradient()
{
    int w = getWidth();
    int h = getHeight();
    if (w <= 0 || h <= 0) return;

    // Create a 2D image so we can bake the text and ticks directly into it
    cachedGradient = juce::Image (juce::Image::ARGB, w, h, true);

    // 1. Bake the 30% dimmed continuous gradient
    juce::Image::BitmapData data (cachedGradient, juce::Image::BitmapData::writeOnly);
    for (int x = 0; x < w; ++x)
    {
        float t = static_cast<float>(x) / static_cast<float>(w - 1);
        float midiAtX = minMidi + t * (maxMidi - minMidi);
        
        float chroma = std::fmod (midiAtX, 12.0f);
        if (chroma < 0.0f) chroma += 12.0f;

        juce::Colour fullColor = ColorPalette::getContinuousColor (chroma);
        juce::Colour dimmedColor = juce::Colours::black.interpolatedWith (fullColor, 0.3f);

        for (int y = 0; y < h; ++y) {
            data.setPixelColour (x, y, dimmedColor);
        }
    }

    // 2. Bake the 30% dimmed labels and 1x5 ticks into the image
    juce::Graphics g (cachedGradient);
    g.setFont (juce::Font (static_cast<float>(h) * 0.55f, juce::Font::bold));

    int startMidi = static_cast<int>(std::ceil(minMidi));
    int endMidi = static_cast<int>(std::floor(maxMidi));

    for (int note = startMidi; note <= endMidi; ++note)
    {
        float x = static_cast<float>(w) * ((static_cast<float>(note) - minMidi) / (maxMidi - minMidi));
        
        int chroma = note % 12;
        if (chroma < 0) chroma += 12;
        
        juce::Colour fullColor = ColorPalette::chromaticScale[static_cast<size_t>(chroma)].color;
        juce::Colour dimmedColor = juce::Colours::black.interpolatedWith (fullColor, 0.3f);
        
        g.setColour (dimmedColor);

        // 1x5 tick
        g.fillRect (x - 0.5f, 0.0f, 1.0f, 5.0f);

        // Label
        juce::String name = getNoteName(note);
        g.drawText (name, static_cast<int>(x) - 15, 6, 30, h - 6, juce::Justification::centredTop);
    }
}

void LineTuner::resized()
{
    updateCachedGradient();
}

void LineTuner::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    int w = getWidth();
    int h = getHeight();

    // 1. Draw the fully baked static background (Opaque overwrite, zero alpha tax)
    if (cachedGradient.isValid()) {
        g.drawImageAt (cachedGradient, 0, 0);
    }

    // 2. Dynamic Illumination Overlay
    if (currentMidi >= minMidi && currentMidi <= maxMidi)
    {
        g.setFont (juce::Font (static_cast<float>(h) * 0.55f, juce::Font::bold));

        // Find the nearest perfect whole note to illuminate
        int nearestNote = static_cast<int>(std::round(currentMidi));

        if (nearestNote >= std::ceil(minMidi) && nearestNote <= std::floor(maxMidi))
        {
            float closestX = bounds.getWidth() * ((static_cast<float>(nearestNote) - minMidi) / (maxMidi - minMidi));
            
            int nearestChroma = nearestNote % 12;
            if (nearestChroma < 0) nearestChroma += 12;
            
            juce::Colour nearestColor = ColorPalette::chromaticScale[static_cast<size_t>(nearestChroma)].color;
            
            g.setColour (nearestColor);
            
            // Light up the 1x5 tick
            g.fillRect (closestX - 0.5f, 0.0f, 1.0f, 5.0f);
            
            // Light up the label
            juce::String name = getNoteName(nearestNote);
            g.drawText (name, static_cast<int>(closestX) - 15, 6, 30, h - 6, juce::Justification::centredTop);
        }

        // Draw the exact float pitch indicator line at 100% brightness
        float pitchX = bounds.getWidth() * ((currentMidi - minMidi) / (maxMidi - minMidi));
        
        float exactChroma = std::fmod (currentMidi, 12.0f);
        if (exactChroma < 0.0f) exactChroma += 12.0f;
        
        juce::Colour exactColor = ColorPalette::getContinuousColor (exactChroma);
        
        g.setColour (exactColor);
        g.drawLine (pitchX, 0.0f, pitchX, static_cast<float>(h), 1.0f);
    }
}