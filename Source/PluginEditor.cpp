#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

const std::array<Pitch, 12> PitchengaAudioProcessorEditor::chromaticScale = {{
    { Tone::Do,  0, 16.35, juce::Colour::fromRGB (255, 0, 0), "Red", "C" },
    { Tone::Ra,  1, 17.32, juce::Colour::fromRGB (255, 87, 87), "Pink", "Db" },
    { Tone::Re,  2, 18.35, juce::Colour::fromRGB (255, 87, 0), "Orange", "D" },
    { Tone::Me,  3, 19.45, juce::Colour::fromRGB (171, 127, 0), "Ochre", "Eb" },
    { Tone::Mi,  4, 20.60, juce::Colour::fromRGB (255, 255, 0), "Yellow", "E" },
    { Tone::Fa,  5, 21.83, juce::Colour::fromRGB (0, 255, 0), "Green", "F" },
    { Tone::Fi,  6, 23.12, juce::Colour::fromRGB (0, 171, 127), "Teal", "F#" },
    { Tone::So,  7, 24.50, juce::Colour::fromRGB (0, 255, 255), "Cyan", "G" },
    { Tone::Le,  8, 25.96, juce::Colour::fromRGB (87, 87, 223), "Lilac", "Ab" },
    { Tone::La,  9, 27.50, juce::Colour::fromRGB (0, 0, 255), "Blue", "A" },
    { Tone::Te, 10, 29.14, juce::Colour::fromRGB (127, 0, 255), "Violet", "Bb" },
    { Tone::Ti, 11, 30.87, juce::Colour::fromRGB (255, 0, 255), "Magenta", "B" }
}};

// --- CqtWorkerThread Implementation ---

PitchengaAudioProcessorEditor::CqtWorkerThread::CqtWorkerThread (PitchengaAudioProcessor& p)
    : Thread ("CqtWorker"), processor (p)
{
    setupBuffers();
}

void PitchengaAudioProcessorEditor::CqtWorkerThread::setupBuffers()
{
    CqtEngine::Config config;
    config.octaves = PitchengaAudioProcessor::numOctaves;
    config.samplingFreq = processor.getSampleRate() > 0 ? processor.getSampleRate() : 44100.0;
    cqt.updateConfig (config);
    cqt.init();

    const int totalBins = cqt.getTotalBins();
    const int binsPerOctave = cqt.getBinsPerOctave();
    const size_t signalBlockSize = static_cast<size_t>(cqt.getSignalBlockSize());

    pcDetector = std::make_unique<HarmonicPatternPitchClassDetector> (binsPerOctave, config.binsPerHalftone);
    spectralEqualizer = std::make_unique<SpectralEqualizer> (totalBins, 30);
    allBinSmoother = std::make_unique<ExpSmoother> (totalBins, 0.2);
    octaveBinSmoother = std::make_unique<ExpSmoother> (binsPerOctave, 0.2);

    workBuffer.assign (signalBlockSize, 0.0f);
    
    // Initialize sliding windows for each octave
    slidingWindows.assign (static_cast<size_t> (PitchengaAudioProcessor::numOctaves),
                           std::vector<float>(signalBlockSize, 0.0f));

    cqtSpectrum.resize (static_cast<size_t> (cqt.getKernelBins()));
    amplitudeSpectrumDb.assign (static_cast<size_t> (totalBins), 0.0);
    octaveBins.assign (static_cast<size_t> (binsPerOctave), 0.0);
    
    {
        const juce::CriticalSection::ScopedLockType lock (resultLock);
        results.assign (static_cast<size_t> (binsPerOctave), 0.0);
    }
}

void PitchengaAudioProcessorEditor::CqtWorkerThread::updateSampleRate (double newSampleRate)
{
    if (newSampleRate > 0 && std::abs (newSampleRate - cqt.getConfig().samplingFreq) > 0.01)
    {
        setupBuffers();
    }
}

double PitchengaAudioProcessorEditor::CqtWorkerThread::amplitudeToDbRescaled (double amplitude)
{
    // Exact port of HarmonEye's DecibelCalculator.java
    constexpr double zeroAmplitudeDb = -90.30899869919436;
    constexpr double zeroAmplitudeDbInv = 1.0 / zeroAmplitudeDb;

    if (amplitude <= 0.00003051757) return 0.0;

    double db = 20.0 * std::log10 (amplitude);
    return std::max (0.0, 1.0 - (db * zeroAmplitudeDbInv));
}

void PitchengaAudioProcessorEditor::CqtWorkerThread::run()
{
    while (! threadShouldExit())
    {
        updateSampleRate (processor.getSampleRate());

        auto& octaves = processor.getOctaves();
        const int signalBlockSize = cqt.getSignalBlockSize();
        const int binsPerOctave = cqt.getBinsPerOctave();

        if (signalBlockSize <= 0 || slidingWindows.empty()) { wait(5); continue; }

        // --- THE ARCHITECTURAL FIX ---
        // Process exactly 1024 samples at a time. This divorces the DSP from the DAW buffer size,
        // preventing skipped frames and double-ticked smoothers.
        while (octaves[0].fifo.getNumReady() >= 1024)
        {
            for (int oct = 0; oct < PitchengaAudioProcessor::numOctaves; ++oct)
            {
                auto& fifo = octaves[static_cast<size_t> (oct)].fifo;
                auto& buffer = octaves[static_cast<size_t> (oct)].buffer;
                auto& win = slidingWindows[static_cast<size_t> (oct)];

                // Calculate exactly how many samples this octave should process
                // Oct 0: 1024, Oct 1: 512, Oct 2: 256...
                int samplesWanted = 1024 >> oct;

                int start1, size1, start2, size2;
                fifo.prepareToRead (samplesWanted, start1, size1, start2, size2);
                int actualRead = size1 + size2;

                if (actualRead > 0)
                {
                    // Shift the sliding window left
                    std::memmove (win.data(), win.data() + actualRead,
                                  static_cast<size_t> (signalBlockSize - actualRead) * sizeof (float));

                    // Append the cleanly extracted data
                    if (size1 > 0) std::copy (buffer.begin() + start1, buffer.begin() + start1 + size1,
                                             win.begin() + (static_cast<ptrdiff_t>(signalBlockSize) - actualRead));
                    if (size2 > 0) std::copy (buffer.begin() + start2, buffer.begin() + start2 + size2,
                                             win.begin() + (static_cast<ptrdiff_t>(signalBlockSize) - actualRead) + size1);

                    fifo.finishedRead (actualRead);
                }
            }

            // Perform the CQT on the perfectly aligned overlapping windows!
            for (int oct = 0; oct < PitchengaAudioProcessor::numOctaves; ++oct)
            {
                const auto& win = slidingWindows[static_cast<size_t>(oct)];
                if (win.size() == static_cast<size_t>(signalBlockSize))
                {
                    cqt.transform (win, cqtSpectrum);
                    
                    // --- THE THRESHOLD FIX ---
                    // Adjust this gain until your attacks feel instantly snappy again.
                    const double inputGain = 8.0;

                    int startIndex = (PitchengaAudioProcessor::numOctaves - 1 - oct) * binsPerOctave;
                    for (size_t i = 0; i < cqtSpectrum.size(); ++i) {
                        double amplitude = std::abs (cqtSpectrum[i]) * inputGain;
                        amplitudeSpectrumDb[static_cast<size_t> (startIndex) + i] = amplitudeToDbRescaled (amplitude);
                    }
                }
            }

            const auto& filteredSpectrum = allBinSmoother->smooth (amplitudeSpectrumDb);
            const auto& detectedPitchClasses = pcDetector->detectPitchClasses (filteredSpectrum);
            const auto& equalizedPitchClasses = spectralEqualizer->filter (detectedPitchClasses);

            std::fill (octaveBins.begin(), octaveBins.end(), 0.0);
            for (int i = 0; i < binsPerOctave; ++i) {
                double maxVal = 0.0;
                for (int j = i; j < static_cast<int> (equalizedPitchClasses.size()); j += binsPerOctave) {
                    maxVal = std::max (maxVal, equalizedPitchClasses[static_cast<size_t> (j)]);
                }
                octaveBins[static_cast<size_t> (i)] = maxVal;
            }

            const auto& smoothed = octaveBinSmoother->smooth (octaveBins);
            // const auto& smoothed = octaveBins;

            {
                const juce::CriticalSection::ScopedLockType lock (resultLock);
                if (results.size() == smoothed.size())
                {
                    std::copy (smoothed.begin(), smoothed.end(), results.begin());
                    newDataAvailable.store (true);
                }
            }
        }

        wait (5); 
    }
}

void PitchengaAudioProcessorEditor::CqtWorkerThread::getLatestResults (std::vector<double>& dest)
{
    const juce::CriticalSection::ScopedLockType lock (resultLock);
    if (dest.size() != results.size())
        dest.resize (results.size());
    std::copy (results.begin(), results.end(), dest.begin());
}

// --- Editor Implementation ---

PitchengaAudioProcessorEditor::PitchengaAudioProcessorEditor (PitchengaAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), worker (p)
{
    smoothedOctaveBins.resize (static_cast<size_t> (totalFoldedBins), 0.0);
    
    setSize (600, 600);
    worker.startThread (juce::Thread::Priority::high);
    startTimerHz (48);
}

PitchengaAudioProcessorEditor::~PitchengaAudioProcessorEditor()
{
    worker.stopThread (2000);
    stopTimer();
}

void PitchengaAudioProcessorEditor::timerCallback()
{
    if (worker.hasNewData())
    {
        worker.getLatestResults (smoothedOctaveBins);
        worker.clearNewDataFlag();
        repaint();
    }
}

juce::Colour PitchengaAudioProcessorEditor::calculateColor (float velocity, float toneRatio)
{
    // 1. NO std::fmod() NEEDED.
    // toneRatio only ever ranges from -0.444 to 11.444.
    // A single addition handles the negative wrap perfectly.
    float wrappedRatio = toneRatio;
    if (wrappedRatio < 0.0f) wrappedRatio += 12.0f;

    int toneNumber = static_cast<int> (std::floor (wrappedRatio));
    float diff = wrappedRatio - static_cast<float> (toneNumber);

    // 2. NO MODULO NEEDED.
    // wrappedRatio is strictly [0.0, 12.0), so toneNumber is strictly 0 to 11.
    int currentIdx = toneNumber;
    juce::Colour toneColor = chromaticScale[static_cast<size_t>(currentIdx)].color;

    // --- Port of getGuessAndPitchinessColor & transposePitch ---
    juce::Colour guessColor;
    if (std::abs(diff) < 0.000000000042f)
    {
        guessColor = toneColor;
    }
    else
    {
        // Transpose -1 or +1 step depending on diff direction
        int pitchyIdx = diff < 0 ? (currentIdx - 1) : (currentIdx + 1);
        if (pitchyIdx < 0) pitchyIdx += 12;
        pitchyIdx %= 12;

        // 3. NO MODULO NEEDED.
        // It only moves by exactly 1 step, so a simple bounds check wraps it perfectly.
        if (pitchyIdx < 0) pitchyIdx = 11;
        else if (pitchyIdx > 11) pitchyIdx = 0;

        juce::Colour pitchyColor = chromaticScale[static_cast<size_t>(pitchyIdx)].color;

        // Simple approximation of the ordinal pitchinessDiff logic
        float pitchinessDiff = std::abs(diff);
        guessColor = toneColor.interpolatedWith (pitchyColor, pitchinessDiff);
    }

    float colorVelocity = 0.3f + (velocity * 1.2f);
    if (colorVelocity > 1.0f) colorVelocity = 1.0f;

    return juce::Colours::black.interpolatedWith (guessColor, colorVelocity);
}

void PitchengaAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);

    auto bounds = getLocalBounds().toFloat();
    auto center = bounds.getCentre();
    auto baseRadius = std::min (bounds.getWidth(), bounds.getHeight()) * 0.45f;

    // 1. Sort to get bin ranks (Matching Java indexToVelocityPairs)
    struct BinData { int index; float velocity; };
    std::vector<BinData> sortedBins (totalFoldedBins);
    for (int i = 0; i < totalFoldedBins; ++i) {
        sortedBins[i] = { i, static_cast<float>(smoothedOctaveBins[i]) };
    }

    std::sort (sortedBins.begin(), sortedBins.end(), [](const BinData& a, const BinData& b) {
        return a.velocity < b.velocity;
    });

    std::vector<int> binOrders (totalFoldedBins);
    for (int rank = 0; rank < totalFoldedBins; ++rank) {
        binOrders[sortedBins[rank].index] = rank;
    }

    int biggestBinNumber = sortedBins.back().index;

    // 2. Render Loop
    for (int i = 0; i < totalFoldedBins; ++i)
    {
        float rawVelocity = static_cast<float>(smoothedOctaveBins[i]);

        // Rank-based amplification
        float renderVelocity = rawVelocity * (binOrders[i] * 0.008f);
        if (i == biggestBinNumber) {
            renderVelocity *= 1.3f;
        }
        renderVelocity = std::min (renderVelocity, 1.15f);

        float toneRatio = static_cast<float> (i) / static_cast<float> (binsPerSemitone);
        juce::Colour color = calculateColor (renderVelocity, toneRatio);

        // Use the amplified velocity for radius
        float currentRadius = baseRadius * renderVelocity;

        auto& originalPath = segmentPaths[static_cast<size_t>(i)];
        auto transform = juce::AffineTransform::scale (currentRadius, currentRadius).translated (center.x, center.y);

        g.setColour (color);
        g.fillPath (originalPath, transform);

        g.strokePath (originalPath, strokeType, transform);
    }
}

void PitchengaAudioProcessorEditor::resized()
{
    constexpr float angleStep = juce::MathConstants<float>::twoPi / static_cast<float>(totalFoldedBins);
    const float rotation = 0.0f - (0.5f * angleStep);

    for (int i = 0; i < totalFoldedBins; ++i)
    {
        const float startAngle = static_cast<float>(i) * angleStep + rotation;
        const float endAngle = static_cast<float>(i + 1) * angleStep + rotation;

        juce::Path p;
        p.addCentredArc (0.0f, 0.0f, 1.0f, 1.0f, 0.0f, startAngle, endAngle, true);
        p.lineTo (0.0f, 0.0f);
        p.closeSubPath();

        segmentPaths[static_cast<size_t>(i)] = p;
    }
}
