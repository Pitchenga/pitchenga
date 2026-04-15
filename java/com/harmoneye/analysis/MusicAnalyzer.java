package com.harmoneye.analysis;

import com.harmoneye.audio.DecibelCalculator;
import com.harmoneye.audio.MultiRateRingBufferBank;
import com.harmoneye.audio.SoundConsumer;
import com.harmoneye.math.cqt.CqtContext;
import com.harmoneye.math.cqt.FastCqt;
import com.harmoneye.math.matrix.ComplexVector;
import com.harmoneye.math.matrix.DComplex;
import com.harmoneye.viz.Visualizer;
import org.apache.commons.math3.stat.descriptive.rank.Median;
import org.apache.commons.math3.util.FastMath;

import java.util.concurrent.atomic.AtomicBoolean;

public class MusicAnalyzer implements SoundConsumer {

    //fixme: Un-hack
    public volatile static MusicAnalyzer INSTANCE;

    /**
     * [0.0; 1.0] 1.0 = no smoothing
     */
//    private static final double SMOOTHING_FACTOR = 0.25;
    private static final double SMOOTHING_FACTOR = 0.2;

    private final CqtContext ctx;

    private final FastCqt cqt;
    private final MultiRateRingBufferBank ringBufferBank;
    private final DecibelCalculator dbCalculator;
    private final HarmonicPatternPitchClassDetector pcDetector;
    private final Visualizer<AnalyzedFrame> visualizer;
    private final Visualizer<AnalyzedFrame> visualizer2;
    //private MovingAverageAccumulator accumulator;
    private final ExpSmoother accumulator;
    private final ExpSmoother allBinSmoother;
    private final ExpSmoother octaveBinSmoother;
    private NoiseGate noiseGate;
    private final PercussionSuppressor percussionSuppressor;
    private final SpectralEqualizer spectralEqualizer;
    private Median medianFilter;

    private final double[] samples;
    /**
     * peak amplitude spectrum
     */
    private final double[] amplitudeSpectrumDb;
    private final double[] octaveBins;

    private final AtomicBoolean initialized = new AtomicBoolean();
    private final AtomicBoolean accumulatorEnabled = new AtomicBoolean();
//    private final AtomicBoolean octaveBinSmootherEnabled = new AtomicBoolean(true);

//    private static final boolean BIN_SMOOTHER_ENABLED = false;
//        private static final boolean BIN_SMOOTHER_ENABLED = true;
//    private static final boolean OCTAVE_BIN_SMOOTHER_ENABLED = true;
//        private static final boolean OCTAVE_BIN_SMOOTHER_ENABLED = false;
    private static final boolean HARMONIC_DETECTOR_ENABLED = true;
    //    private static final boolean HARMONIC_DETECTOR_ENABLED = false;
//    private static final boolean PERCUSSION_SUPPRESSOR_ENABLED = true;
    private static final boolean PERCUSSION_SUPPRESSOR_ENABLED = false;
    private static final boolean SPECTRAL_EQUALIZER_ENABLED = true;
    //    private static final boolean SPECTRAL_EQUALIZER_ENABLED = false;
    //    private static final boolean SPECTRAL_EQUALIZER_ENABLED = false;
    private static final boolean NOISE_GATE_ENABLED = false;
    //        private static final boolean NOISE_GATE_ENABLED = true;
    private static final boolean NOISE_GATE_MEDIAN_THRESHOLD_ENABLED = false;
//    private static final boolean NOISE_GATE_MEDIAN_THRESHOLD_ENABLED = true;

    public MusicAnalyzer(Visualizer<AnalyzedFrame> visualizer,
                         float sampleRate, int bitsPerSample, Visualizer<AnalyzedFrame> visualizer2) {
        INSTANCE = this;
        this.visualizer = visualizer;
        this.visualizer2 = visualizer2;

        ctx = CqtContext.create()
                .samplingFreq(sampleRate)
//                .maxFreq((2 << 6) * 65.4063913251)
//                .octaves(2)
//                .octaves(8)
                .kernelOctaves(1)
                .binsPerHalftone(9)
                .build();

        samples = new double[ctx.getSignalBlockSize()];
        amplitudeSpectrumDb = new double[ctx.getTotalBins()];
        octaveBins = new double[ctx.getBinsPerOctave()];

        ringBufferBank = new MultiRateRingBufferBank(ctx.getSignalBlockSize(), ctx.getOctaves());
        dbCalculator = new DecibelCalculator(bitsPerSample);
        pcDetector = new HarmonicPatternPitchClassDetector(ctx);
        octaveBinSmoother = new ExpSmoother(ctx.getBinsPerOctave(), SMOOTHING_FACTOR);
        allBinSmoother = new ExpSmoother(ctx.getTotalBins(), SMOOTHING_FACTOR);
        //accumulator = new MovingAverageAccumulator(ctx.getBinsPerOctave());
        accumulator = new ExpSmoother(ctx.getBinsPerOctave(), 0.005);
        if (NOISE_GATE_ENABLED) {
            noiseGate = new NoiseGate(ctx.getBinsPerOctave());
            if (NOISE_GATE_MEDIAN_THRESHOLD_ENABLED) {
                medianFilter = new Median();
            }
        }
        percussionSuppressor = new PercussionSuppressor(ctx.getTotalBins(), 7);
        spectralEqualizer = new SpectralEqualizer(ctx.getTotalBins(), 30);

        cqt = new FastCqt(ctx);
    }

    public void init() {
        cqt.init();
        initialized.set(true);
    }

    @Override
    public void consume(double[] samples) {
        ringBufferBank.write(samples);
    }

    public void updateSignal() {
        if (!initialized.get()) {
            return;
        }
        computeCqtSpectrum();
        AnalyzedFrame frame = analyzeFrame(amplitudeSpectrumDb);
        visualizer.update(frame);
        if (visualizer2 != null) {
            visualizer2.update(frame);
        }
    }

    private void computeCqtSpectrum() {
        int startIndex = (ctx.getOctaves() - 1) * ctx.getBinsPerOctave();
        for (int octave = 0; octave < ctx.getOctaves(); octave++, startIndex -= ctx.getBinsPerOctave()) {
            ringBufferBank.readLast(octave, samples.length, samples);
            ComplexVector cqtSpectrum = cqt.transform(samples);
            toAmplitudeDbSpectrum(cqtSpectrum, amplitudeSpectrumDb, startIndex);
        }
    }

    private void toAmplitudeDbSpectrum(ComplexVector cqtSpectrum, double[] amplitudeSpectrum, int startIndex) {
        double[] elements = cqtSpectrum.getElements();
        for (int i = 0, index = 0; i < cqtSpectrum.size(); i++, index += 2) {
            double re = elements[index];
            double im = elements[index + 1];
            double amplitude = DComplex.abs(re, im);
            double amplitudeDb = dbCalculator.amplitudeToDb(amplitude);
            double value = dbCalculator.rescale(amplitudeDb);
            amplitudeSpectrum[startIndex + i] = value;
        }
    }

    private AnalyzedFrame analyzeFrame(double[] amplitudeSpectrumDb) {
        double[] detectedPitchClasses = null;

        amplitudeSpectrumDb = allBinSmoother.smooth(amplitudeSpectrumDb);

        if (PERCUSSION_SUPPRESSOR_ENABLED) {
            amplitudeSpectrumDb = percussionSuppressor.filter(amplitudeSpectrumDb);
        }

        if (HARMONIC_DETECTOR_ENABLED) {
            detectedPitchClasses = pcDetector.detectPitchClasses(amplitudeSpectrumDb);
            if (SPECTRAL_EQUALIZER_ENABLED) {
                detectedPitchClasses = spectralEqualizer.filter(detectedPitchClasses);
            }
            aggregateIntoOctaves(detectedPitchClasses, octaveBins);
        } else {
            aggregateIntoOctaves(amplitudeSpectrumDb, octaveBins);
        }

        double[] smoothedOctaveBins = smooth(octaveBins);

        if (NOISE_GATE_ENABLED) {
            if (NOISE_GATE_MEDIAN_THRESHOLD_ENABLED) {
                double medianValue = medianFilter.evaluate(smoothedOctaveBins);
                noiseGate.setOpenThreshold(medianValue * 1.25);
            }
            noiseGate.filter(smoothedOctaveBins);
        }

        return new AnalyzedFrame(ctx, amplitudeSpectrumDb, smoothedOctaveBins, detectedPitchClasses);
    }

    private void aggregateIntoOctaves(double[] bins, double[] octaveBins) {
        int binsPerOctave = ctx.getBinsPerOctave();
        for (int i = 0; i < binsPerOctave; i++) {
            // maximum over octaves:
            double value = 0;
            for (int j = i; j < bins.length; j += binsPerOctave) {
                value = FastMath.max(value, bins[j]);
            }
            octaveBins[i] = value;
        }

    }

    private double[] smooth(double[] octaveBins) {
        boolean octaveBinSmootherEnabled = true;
//        boolean octaveBinSmootherEnabled = false;
//        boolean octaveBinSmootherEnabled.set(true);
//        octaveBinSmootherEnabled.set(false);
        double[] smoothedOctaveBins;
        double[] accumulatedOctaveBins = accumulator.smooth(octaveBins);
        if (accumulatorEnabled.get()) {
            //accumulator.add(octaveBins);
            //smoothedOctaveBins = accumulator.getAverage();
            smoothedOctaveBins = accumulatedOctaveBins;
        } else if (octaveBinSmootherEnabled) {
            smoothedOctaveBins = octaveBinSmoother.smooth(octaveBins);
        } else {
            smoothedOctaveBins = octaveBins;
        }
        return smoothedOctaveBins;
    }

    public void toggleAccumulatorEnabled() {
        accumulatorEnabled.set(!accumulatorEnabled.get());
//        if (accumulatorEnabled.get()) {
        //accumulator.reset();
//        }
    }

}