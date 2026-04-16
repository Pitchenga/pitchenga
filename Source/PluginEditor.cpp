#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

// Ported chromatic colors from Java HarmonEye - Updated with legacy Tone names
const std::array<Pitch, 12> PitchengaAudioProcessorEditor::chromaticScale = {{
    { Tone::Do,  0, 16.35, juce::Colour::fromRGB (255, 0, 0) },    // C (Red)
    { Tone::Ra,  1, 17.32, juce::Colour::fromRGB (100, 42, 0) },   // Db (Brown)
    { Tone::Re,  2, 18.35, juce::Colour::fromRGB (255, 87, 0) },   // D (Orange)
    { Tone::Me,  3, 19.45, juce::Colour::fromRGB (171, 127, 0) },  // Eb (Ochre)
    { Tone::Mi,  4, 20.60, juce::Colour::fromRGB (255, 255, 0) },  // E (Yellow)
    { Tone::Fa,  5, 21.83, juce::Colour::fromRGB (0, 255, 0) },    // F (Green)
    { Tone::Fi,  6, 23.12, juce::Colour::fromRGB (0, 127, 127) },  // F# (Teal)
    { Tone::So,  7, 24.50, juce::Colour::fromRGB (0, 255, 255) },  // G (Cyan)
    { Tone::Le,  8, 25.96, juce::Colour::fromRGB (0, 42, 137) },   // Ab (Navy)
    { Tone::La,  9, 27.50, juce::Colour::fromRGB (0, 0, 255) },    // A (Blue)
    { Tone::Te, 10, 29.14, juce::Colour::fromRGB (127, 0, 255) },  // Bb (Violet)
    { Tone::Ti, 11, 30.87, juce::Colour::fromRGB (255, 0, 255) }   // B (Magenta)
}};

PitchengaAudioProcessorEditor::PitchengaAudioProcessorEditor (PitchengaAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    fftData.resize (fftSize * 2, 0.0f);
    fifoWorkBuffer.resize (fftSize, 0.0f);
    currentBins.fill (0.0f);
    smoothedBins.fill (0.0f);

    setSize (600, 600);
    startTimerHz (30);
}

PitchengaAudioProcessorEditor::~PitchengaAudioProcessorEditor()
{
    stopTimer();
}

void PitchengaAudioProcessorEditor::timerCallback()
{
    auto& fifo = audioProcessor.getFifo();
    bool dataProcessed = false;

    while (fifo.getNumReady() >= fftSize)
    {
        int start1, size1, start2, size2;
        fifo.prepareToRead (fftSize, start1, size1, start2, size2);

        if (size1 + size2 < fftSize)
            break;

        const auto& buffer = audioProcessor.getFifoBuffer();
        if (size1 > 0) std::copy (buffer.begin() + start1, buffer.begin() + start1 + size1, fifoWorkBuffer.begin());
        if (size2 > 0) std::copy (buffer.begin() + start2, buffer.begin() + start2 + size2, fifoWorkBuffer.begin() + size1);

        fifo.finishedRead (size1 + size2);

        processFft();
        dataProcessed = true;

        // If we are falling behind (e.g. UI lag), skip to the most recent data to maintain sync
        int numReady = fifo.getNumReady();
        if (numReady > fftSize * 2)
        {
            fifo.finishedRead (numReady - fftSize);
        }
    }

    if (dataProcessed)
        repaint();
}

void PitchengaAudioProcessorEditor::updateBinLookupTable (double sampleRate)
{
    activeBinMappings.clear();
    const double binWidth = sampleRate / fftSize;

    for (int i = 1; i < fftSize / 2; ++i)
    {
        double freq = i * binWidth;

        // Skip sub-audio frequencies that would crash the log2 function
        if (freq < 20.0) continue;

        // 1. Find exactly how many semitones we are above C0
        double semitones = static_cast<double>(semitonesPerOctave) * std::log2 (freq / frequencyC0);

        // 2. Multiply by binsPerSemitone and wrap to the 108-bin circle
        int binIndex = static_cast<int> (std::round (semitones * binsPerSemitone)) % totalFoldedBins;

        // Handle negative wraps if the math dips below C0
        if (binIndex < 0) binIndex += totalFoldedBins;

        activeBinMappings.push_back ({ i, binIndex });
    }
}

void PitchengaAudioProcessorEditor::processFft()
{
    const double sampleRate = audioProcessor.getSampleRate();
    if (sampleRate > 0 && std::abs (sampleRate - lastSampleRate) > 0.01)
    {
        updateBinLookupTable (sampleRate);
        lastSampleRate = sampleRate;
    }

    if (lastSampleRate <= 0) return;

    std::fill (fftData.begin(), fftData.end(), 0.0f);
    std::copy (fifoWorkBuffer.begin(), fifoWorkBuffer.end(), fftData.begin());

    window.multiplyWithWindowingTable (fftData.data(), fftSize);
    fft.performFrequencyOnlyForwardTransform (fftData.data());

    currentBins.fill (0.0f);

    for (const auto& mapping : activeBinMappings)
    {
        float magnitude = fftData[static_cast<size_t>(mapping.fftIndex)];
        currentBins[static_cast<size_t>(mapping.binIndex)] = std::max (currentBins[static_cast<size_t>(mapping.binIndex)], magnitude);
    }

    // Exponential Smoothing
    for (int i = 0; i < totalFoldedBins; ++i)
    {
        const auto idx = static_cast<size_t>(i);
        smoothedBins[idx] = (smoothingFactor * currentBins[idx]) + ((1.0f - smoothingFactor) * smoothedBins[idx]);
    }
}

juce::Colour PitchengaAudioProcessorEditor::calculateColor (float velocity, float toneRatio)
{
    int toneNumber = static_cast<int> (std::floor (toneRatio));
    float diff = toneRatio - static_cast<float> (toneNumber);

    int currentIdx = toneNumber % 12;
    int nextIdx = (currentIdx + 1) % 12;

    juce::Colour currentToneColor = chromaticScale[static_cast<size_t>(currentIdx)].color;
    juce::Colour nextToneColor = chromaticScale[static_cast<size_t>(nextIdx)].color;

    // Smoothly interpolate between semitone colors
    juce::Colour baseColor = currentToneColor.interpolatedWith (nextToneColor, diff);

    float colorVelocity = juce::jlimit (0.0f, 1.0f, 0.3f + velocity * 1.5f);
    return juce::Colours::black.interpolatedWith (baseColor, colorVelocity);
}

void PitchengaAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);

    auto bounds = getLocalBounds().toFloat();
    auto center = bounds.getCentre();
    auto baseRadius = std::min (bounds.getWidth(), bounds.getHeight()) * 0.45f;

    for (int i = 0; i < totalFoldedBins; ++i)
    {
        float velocity = smoothedBins[static_cast<size_t>(i)];

        // i goes from 0 to 107.
        // toneRatio will accurately go from 0.0 to 11.888...
        float toneRatio = static_cast<float> (i) / static_cast<float> (binsPerSemitone);

        // Calculate the exact interpolated color across the 12 chromatic scales
        juce::Colour color = calculateColor (velocity, toneRatio);

        // Map energy to both radius and opacity
        float currentRadius = baseRadius * (0.2f + velocity * 0.8f);
        float alpha = juce::jlimit (0.3f, 1.0f, 0.5f + velocity * 0.5f);

        auto& originalPath = segmentPaths[static_cast<size_t>(i)];

        // Applying a transformation directly in fillPath is more efficient than rebuilding the path
        auto transform = juce::AffineTransform::scale (currentRadius, currentRadius).translated (center.x, center.y);

        g.setColour (color.withAlpha (alpha));
        g.fillPath (originalPath, transform);

        // Define segment with a faint outline
        g.setColour (color.withAlpha (0.1f + velocity * 0.2f));
        g.strokePath (originalPath, strokeType, transform);
    }
}

void PitchengaAudioProcessorEditor::resized()
{
    // Divide the 360 degrees by 108
    constexpr float angleStep = juce::MathConstants<float>::twoPi / static_cast<float>(totalFoldedBins);

    for (int i = 0; i < totalFoldedBins; ++i)
    {
        const float startAngle = static_cast<float>(i) * angleStep;
        const float endAngle = static_cast<float>(i + 1) * angleStep;

        juce::Path p;
        // Unit path at origin (0,0) with radius 1.0
        p.addCentredArc (0.0f, 0.0f, 1.0f, 1.0f, 0.0f, startAngle, endAngle, true);
        p.lineTo (0.0f, 0.0f);
        p.closeSubPath();

        segmentPaths[static_cast<size_t>(i)] = p;
    }
}
