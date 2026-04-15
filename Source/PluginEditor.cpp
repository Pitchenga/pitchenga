#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

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
    CqtEngine::Config config;
    config.octaves = PitchengaAudioProcessor::numOctaves;
    config.samplingFreq = 44100.0;
    cqt.updateConfig(config);
    cqt.init();

    int totalBins = cqt.getTotalBins();
    int binsPerOctave = cqt.getBinsPerOctave();

    pcDetector = std::make_unique<HarmonicPatternPitchClassDetector>(binsPerOctave, config.binsPerHalftone);
    spectralEqualizer = std::make_unique<SpectralEqualizer>(totalBins, 30);
    octaveBinSmoother = std::make_unique<ExpSmoother>(binsPerOctave, 0.2);

    workBuffer.resize(cqt.getSignalBlockSize(), 0.0f);
    amplitudeSpectrumDb.resize(totalBins, 0.0);
    octaveBins.resize(binsPerOctave, 0.0);
    smoothedOctaveBins.resize(binsPerOctave, 0.0);

    setSize (600, 600);
    startTimerHz (30);
}

PitchengaAudioProcessorEditor::~PitchengaAudioProcessorEditor()
{
    stopTimer();
}

void PitchengaAudioProcessorEditor::timerCallback()
{
    processCqt();
    repaint();
}

static double amplitudeToDbRescaled(double amplitude) {
    if (amplitude <= 1e-6) return 0.0;
    double db = 20.0 * std::log10(amplitude);
    // Increased range to handle quieter signals
    double rescaled = (db + 80.0) / 80.0; 
    return juce::jlimit(0.0, 1.0, rescaled);
}

void PitchengaAudioProcessorEditor::processCqt()
{
    double sampleRate = audioProcessor.getSampleRate();
    if (sampleRate > 0 && std::abs(sampleRate - cqt.getConfig().samplingFreq) > 0.01) {
        CqtEngine::Config config = cqt.getConfig();
        config.samplingFreq = sampleRate;
        cqt.updateConfig(config);
        cqt.init();
    }

    auto& octaves = audioProcessor.getOctaves();
    int signalBlockSize = cqt.getSignalBlockSize();
    int binsPerOctave = cqt.getBinsPerOctave();

    bool hasData = false;

    for (int oct = 0; oct < cqt.getOctaves(); ++oct)
    {
        auto& fifo = octaves[oct].fifo;
        auto& buffer = octaves[oct].buffer;

        // Process all available data in the FIFO to keep it drained
        while (fifo.getNumReady() >= signalBlockSize)
        {
            int start1, size1, start2, size2;
            fifo.prepareToRead (signalBlockSize, start1, size1, start2, size2);

            if (size1 > 0) std::copy(buffer.begin() + start1, buffer.begin() + start1 + size1, workBuffer.begin());
            if (size2 > 0) std::copy(buffer.begin() + start2, buffer.begin() + start2 + size2, workBuffer.begin() + size1);

            fifo.finishedRead (size1 + size2);

            std::vector<std::complex<float>> cqtSpectrum;
            cqt.transform(workBuffer, cqtSpectrum);

            int startIndex = (cqt.getOctaves() - 1 - oct) * binsPerOctave;
            for (size_t i = 0; i < cqtSpectrum.size(); ++i) {
                double amplitude = std::abs(cqtSpectrum[i]);
                amplitudeSpectrumDb[static_cast<size_t>(startIndex) + i] = amplitudeToDbRescaled(amplitude);
            }
            hasData = true;
        }
    }

    if (!hasData) return;

    // Harmonic Pattern Pitch Class Detector
    std::vector<double> detectedPitchClasses = pcDetector->detectPitchClasses(amplitudeSpectrumDb);
    
    // Spectral Equalizer
    detectedPitchClasses = spectralEqualizer->filter(detectedPitchClasses);

    // Aggregate into octaves
    std::fill(octaveBins.begin(), octaveBins.end(), 0.0);
    for (int i = 0; i < binsPerOctave; ++i) {
        double maxVal = 0.0;
        for (int j = i; j < static_cast<int>(detectedPitchClasses.size()); j += binsPerOctave) {
            maxVal = std::max(maxVal, detectedPitchClasses[j]);
        }
        octaveBins[i] = maxVal;
    }

    // Exponential Smoother
    smoothedOctaveBins = octaveBinSmoother->smooth(octaveBins);
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
        float velocity = static_cast<float>(smoothedOctaveBins[static_cast<size_t>(i)]);

        // Goes from 0 to 107 - toneRatio will accurately go from 0.0 to 11.888...
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
        p.addCentredArc (0.0f, 0.0f, 2.0f, 2.0f, 0.0f, startAngle, endAngle, true);
        p.lineTo (0.0f, 0.0f);
        p.closeSubPath();

        segmentPaths[static_cast<size_t>(i)] = p;
    }
}
