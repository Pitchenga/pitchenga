#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

// Ported chromatic colors from Java HarmonEye - Updated with legacy Tone names
const std::array<Pitch, 12> PitchengaAudioProcessorEditor::chromaticScale = {{
    { Tone::Do,  0, 16.35, juce::Colour::fromRGB (255, 0, 0) },     // C (Red)
    { Tone::Ra,  1, 17.32, juce::Colour::fromRGB (255, 127, 0) },   // Ra (C# / Orange)
    { Tone::Re,  2, 18.35, juce::Colour::fromRGB (255, 255, 0) },   // D (Yellow)
    { Tone::Me,  3, 19.45, juce::Colour::fromRGB (127, 255, 0) },   // Me (D# / Lime)
    { Tone::Mi,  4, 20.60, juce::Colour::fromRGB (0, 255, 0) },     // E (Green)
    { Tone::Fa,  5, 21.83, juce::Colour::fromRGB (0, 255, 127) },   // F (Spring Green)
    { Tone::Fi,  6, 23.12, juce::Colour::fromRGB (0, 255, 255) },   // Fi (F# / Cyan)
    { Tone::So,  7, 24.50, juce::Colour::fromRGB (0, 127, 255) },   // G (Azure)
    { Tone::Le,  8, 25.96, juce::Colour::fromRGB (0, 0, 255) },     // Le (G# / Blue)
    { Tone::La,  9, 27.50, juce::Colour::fromRGB (127, 0, 255) },   // A (Violet)
    { Tone::Te, 10, 29.14, juce::Colour::fromRGB (255, 0, 255) },  // Te (A# / Magenta)
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

    if (fifo.getNumReady() >= fftSize)
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
        repaint();
    }
}

void PitchengaAudioProcessorEditor::processFFT()
{
    std::fill (fftData.begin(), fftData.end(), 0.0f);
    std::copy (fifoWorkBuffer.begin(), fifoWorkBuffer.end(), fftData.begin());

    window.multiplyWithWindowingTable (fftData.data(), fftSize);
    fft.performFrequencyOnlyForwardTransform (fftData.data());

    currentBins.fill (0.0f);

    const double sampleRate = audioProcessor.getSampleRate();
    const double binWidth = sampleRate / fftSize;
    const double refFreq = 16.35159783128741; // C0

    for (int i = 1; i < fftSize / 2; ++i)
    {
        double freq = i * binWidth;
        if (freq < 20.0) continue;

        double semitones = 12.0 * std::log2 (freq / refFreq);
        int binIndex = static_cast<int> (std::round (semitones * 5.0)) % numBins;
        if (binIndex < 0) binIndex += numBins;

        float magnitude = fftData[static_cast<size_t>(i)];
        currentBins[static_cast<size_t>(binIndex)] = std::max (currentBins[static_cast<size_t>(binIndex)], magnitude);
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
    int nextIdx = (currentIdx + (diff < 0 ? -1 : 1)) % 12;
    if (nextIdx < 0) nextIdx += 12;

    juce::Colour currentToneColor = chromaticScale[static_cast<size_t>(currentIdx)].color;
    juce::Colour nextToneColor = chromaticScale[static_cast<size_t>(nextIdx)].color;

    // Smoothly interpolate between semitone colors
    juce::Colour baseColor = currentToneColor.interpolatedWith (nextToneColor, std::abs (diff));

    float colorVelocity = juce::jlimit (0.0f, 1.0f, 0.3f + velocity * 1.5f);
    return juce::Colours::black.interpolatedWith (baseColor, colorVelocity);
}

void PitchengaAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);

    auto bounds = getLocalBounds().toFloat();
    auto center = bounds.getCentre();
    auto radius = std::min (bounds.getWidth(), bounds.getHeight()) * 0.45f;

    const float angleStep = juce::MathConstants<float>::twoPi / static_cast<float>(numBins);

    for (int i = 0; i < numBins; ++i)
    {
        float velocity = smoothedBins[static_cast<size_t>(i)];
        float toneRatio = static_cast<float> (i) / 5.0f; // 5 bins per semitone

        juce::Colour color = calculateColor (velocity, toneRatio);

        float startAngle = static_cast<float>(i) * angleStep;
        float endAngle = static_cast<float>(i + 1) * angleStep;

        juce::Path p;
        p.addCentredArc (center.x, center.y, radius, radius, 0.0f, startAngle, endAngle, true);
        p.lineTo (center);
        p.closeSubPath();

        g.setColour (color);
        g.fillPath (p);

        // Define segment with a faint outline
        g.setColour (color.withAlpha (0.2f));
        g.strokePath (p, juce::PathStrokeType (0.5f));
    }
}

void PitchengaAudioProcessorEditor::resized() {}
