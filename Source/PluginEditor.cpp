#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

// Ported chromatic colors from Java HarmonEye - Updated with legacy Tone names
const std::array<Pitch, 12> PitchengaAudioProcessorEditor::chromaticScale = {{
    { Tone::Do,  0, 16.35, juce::Colour::fromRGB (255, 0, 0) },    // C (Red)
    { Tone::Ra,  1, 17.32, juce::Colour::fromRGB (255, 127, 0) },  // C# (Orange)
    { Tone::Re,  2, 18.35, juce::Colour::fromRGB (255, 255, 0) },  // D (Yellow)
    { Tone::Me,  3, 19.45, juce::Colour::fromRGB (127, 255, 0) },  // D# (Lime)
    { Tone::Mi,  4, 20.60, juce::Colour::fromRGB (0, 255, 0) },    // E (Green)
    { Tone::Fa,  5, 21.83, juce::Colour::fromRGB (0, 255, 127) },  // F (Spring Green)
    { Tone::Fi,  6, 23.12, juce::Colour::fromRGB (0, 255, 255) },  // F# (Cyan)
    { Tone::So,  7, 24.50, juce::Colour::fromRGB (0, 127, 255) },  // G (Azure)
    { Tone::Le,  8, 25.96, juce::Colour::fromRGB (0, 0, 255) },    // G# (Blue)
    { Tone::La,  9, 27.50, juce::Colour::fromRGB (127, 0, 255) },  // A (Violet)
    { Tone::Te, 10, 29.14, juce::Colour::fromRGB (255, 0, 255) },  // A# (Magenta)
    { Tone::Ti, 11, 30.87, juce::Colour::fromRGB (255, 0, 127) }   // B (Rose)
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
        auto scope = fifo.read (fftSize);
        int start1 = scope.startIndex1;
        int size1 = scope.blockSize1;
        int start2 = scope.startIndex2;
        int size2 = scope.blockSize2;

        const auto& buffer = audioProcessor.getFifoBuffer();
        if (size1 > 0) std::copy (buffer.begin() + static_cast<size_t> (start1), buffer.begin() + static_cast<size_t> (start1 + size1), fifoWorkBuffer.begin());
        if (size2 > 0) std::copy (buffer.begin() + static_cast<size_t> (start2), buffer.begin() + static_cast<size_t> (start2 + size2), fifoWorkBuffer.begin() + static_cast<size_t> (size1));

        fifo.finishedRead (size1 + size2);

        processFFT();
        dataProcessed = true;

        // If we are falling behind (e.g. UI lag), skip to the most recent data to maintain sync
        if (fifo.getNumReady() > fftSize * 2)
        {
            fifo.finishedRead (fifo.getNumReady() - fftSize);
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
        if (freq < 20.0) continue;

        double semitones = static_cast<double> (semitonesInOctave) * std::log2 (freq / frequencyC0);
        int binIndex = static_cast<int> (std::round (semitones * binsPerSemitone)) % numBins;
        if (binIndex < 0) binIndex += numBins;

        activeBinMappings.push_back ({ i, binIndex });
    }
}

void PitchengaAudioProcessorEditor::processFFT()
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
    for (int i = 0; i < numBins; ++i)
    {
        size_t idx = static_cast<size_t>(i);
        smoothedBins[idx] = (smoothingFactor * currentBins[idx]) + ((1.0f - smoothingFactor) * smoothedBins[idx]);
    }
}

juce::Colour PitchengaAudioProcessorEditor::calculateColor (float velocity, float toneRatio)
{
    int toneNumber = static_cast<int> (std::floor (toneRatio));
    float diff = toneRatio - static_cast<float> (toneNumber);

    int currentIdx = toneNumber % 12;
    if (currentIdx < 0) currentIdx += 12;
    int nextIdx = (currentIdx + 1) % 12;
    if (nextIdx < 0) nextIdx += 12;

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
    const juce::PathStrokeType stroke (0.5f);

    for (int i = 0; i < numBins; ++i)
    {
        float velocity = smoothedBins[static_cast<size_t>(i)];
        float toneRatio = static_cast<float> (i) / static_cast<float> (binsPerSemitone);

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
        g.strokePath (originalPath, stroke, transform);
    }
}

void PitchengaAudioProcessorEditor::resized()
{
    const float angleStep = juce::MathConstants<float>::twoPi / static_cast<float>(numBins);

    for (int i = 0; i < numBins; ++i)
    {
        float startAngle = static_cast<float>(i) * angleStep;
        float endAngle = static_cast<float>(i + 1) * angleStep;

        juce::Path p;
        // Unit path at origin (0,0) with radius 1.0
        p.addCentredArc (0.0f, 0.0f, 2.0f, 2.0f, 0.0f, startAngle, endAngle, true);
        p.lineTo (0.0f, 0.0f);
        p.closeSubPath();

        segmentPaths[static_cast<size_t>(i)] = p;
    }
}
