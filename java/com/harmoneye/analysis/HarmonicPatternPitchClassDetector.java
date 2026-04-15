package com.harmoneye.analysis;

import com.harmoneye.math.cqt.CqtContext;
import org.apache.commons.math3.stat.descriptive.rank.Median;
import org.apache.commons.math3.util.FastMath;

public class HarmonicPatternPitchClassDetector {

    // Better signal/noise than default below
    private static final int DEFAULT_HARMONIC_COUNT = 10;
    private static final double HARMONIC_WEIGHT_FALLOFF = 0.5;
//    private static final int DEFAULT_HARMONIC_COUNT = 6;
//    private static final double HARMONIC_WEIGHT_FALLOFF = 0.4;

    // private ScalarExpSmoother smoother = new ScalarExpSmoother(0.1);

    private final int harmonicCount;
    private final int binsPerOctave;
    private final int binsPerHalftoneHalf;
    private final double baseFreqInv;
    private final double harmonicCountMinusOneInv;

    private double[] harmonicBins;
    private final int[] harmonicBinsIndexes;
    private final CqtContext ctx;
    private final Median medianStat = new Median();

    public HarmonicPatternPitchClassDetector(CqtContext ctx) {
        this(ctx, DEFAULT_HARMONIC_COUNT);
    }

    public HarmonicPatternPitchClassDetector(CqtContext ctx, int harmonicCount) {
        this.ctx = ctx;
        this.harmonicCount = harmonicCount;
        binsPerOctave = ctx.getBinsPerOctave();
        binsPerHalftoneHalf = ctx.getBinsPerHalftone() / 2;
        double baseFreq = ctx.getBaseFreq();
        baseFreqInv = 1.0 / baseFreq;

        harmonicBinsIndexes = new int[harmonicCount];
        for (int i = 0; i < harmonicCount; i++) {
            harmonicBinsIndexes[i] = harmonicFreqBin(ctx.getBaseFreq(), i + 1);
        }

        harmonicCountMinusOneInv = 1.0 / (harmonicCount - 1);
    }

    /**
     * detect fundamental frequencies for bins within pitch classes via pattern
     * matching of the harmonics
     *
     * @param cqBins
     * @return
     */
    public double[] detectPitchClasses(double[] cqBins) {
        int size = cqBins.length;

        if (harmonicBins == null) {
            harmonicBins = new double[size];
        }

        for (int i = 0; i < size; i++) {
            harmonicBins[i] = extractHarmonics(cqBins, i, harmonicCount);
        }

        // normalizeViaMax(cqBins, octaveBins);
        normalizeViaMean(cqBins, harmonicBins);
        // normalizeViaMedian(cqBins, harmonicBins);

        return harmonicBins;
    }

    private void normalizeViaMedian(double[] origBins, double[] harmonicBins) {
        double harmonicMedian = medianStat.evaluate(harmonicBins);
        double origMedian = medianStat.evaluate(origBins);
        // System.out.println(harmonicMedian + " " + origMedian);

        double normalizationFactor = origMedian / harmonicMedian;
        int size = harmonicBins.length;
        for (int i = 0; i < size; i++) {
            harmonicBins[i] *= normalizationFactor;
        }
    }

    private void normalizeViaMean(double[] origBins, double[] harmonicBins) {
        int size = harmonicBins.length;

        double harmonicSum = 0;
        double origSum = 0;

        for (int i = 0; i < size; i++) {
            harmonicSum += harmonicBins[i];
            origSum += origBins[i];
        }

        if (harmonicSum > 0) {
            double normalizationFactor = origSum / harmonicSum;
            for (int i = 0; i < size; i++) {
                harmonicBins[i] *= normalizationFactor;
            }
        }
    }

    private void normalizeViaMax(double[] cqBins, double[] octaveBins) {

        // TODO: this kind of normalization is bad and unstable

        double harmonicMax = 0;
        double cqMax = 0;
        for (int i = 0; i < octaveBins.length; i++) {
            harmonicMax = FastMath.max(harmonicMax, octaveBins[i]);
            cqMax = FastMath.max(cqMax, cqBins[i]);
        }

        if (harmonicMax > 0) {
            double factor = cqMax < 1 ? cqMax / harmonicMax : 1 / cqMax;
            // double smoothedFactor = smoother.smooth(factor);
            for (int i = 0; i < octaveBins.length; i++) {
                octaveBins[i] *= factor;
            }
        }
    }

    private double extractHarmonics(double[] cqBins, int baseFreqBin, int harmonicCount) {
        double dotProduct = cqBins[baseFreqBin];
        int centerFreqBin = baseFreqBin - ctx.getBinsPerHalftone() / 2;
        for (int i = 1; i <= harmonicCount; i++) {
            int harmonicBin = centerFreqBin + harmonicBinsIndexes[i - 1];
            if (harmonicBin < cqBins.length) {
                double weight = 1 - HARMONIC_WEIGHT_FALLOFF * (i - 1)
                        * harmonicCountMinusOneInv;
                // TODO: interpolate the bin values using a continuous bin index
                dotProduct += weight * cqBins[harmonicBin];
            }
        }
        return dotProduct;
    }

    /**
     * @param frequency     of the first harmonic
     * @param harmonicIndex one-based index of the desired k-th harmonic
     * @return zero-based cq bin index of the k-th harmonic frequency
     */
    private int harmonicFreqBin(double frequency, int harmonicIndex) {
        return (int) FastMath.round(binsPerOctave
                * FastMath.log(2, harmonicIndex * frequency * baseFreqInv))
                + binsPerHalftoneHalf;
    }
}