package com.harmoneye.audio;

import org.apache.commons.math3.util.FastMath;

public class DecibelCalculator {

    /**
     * Amplitude step for digital signal. Also minimum amplitude greater than
     * zero.
     */
    private final double amplitudeStep;
    /**
     * Decibels for zero amplitude
     */
    private final double zeroAmplitudeDb;
    private final double zeroAmplitudeDbInv;

    public DecibelCalculator(int bitsPerSample) {
        // maximum value for unsigned digital amplitude
        long maxSampleValue = 2 << (bitsPerSample - 1);
        amplitudeStep = 1.0 / maxSampleValue;
        zeroAmplitudeDb = toDecibel(amplitudeStep);
        zeroAmplitudeDbInv = 1.0 / zeroAmplitudeDb;
    }

    /**
     * Converts linear digital amplitude to decibels.
     * <p>
     * Since zero amplitude has -Infinity logarithm it is
     * trimmed to
     *
     * @param amplitude linear amplitude scaled to [0.0; 1.0]
     * @return logarithmic amplitude in decibels [DB_THRESHOLD; 0]
     */
    public double amplitudeToDb(double amplitude) {
        // Since reference amplitude is 1, this code is implied:
        // double referenceAmplitude = 1;
        // amplitude /= referenceAmplitude;

        // if amplitude is almost zero, use precomputed min threshold value
        if (amplitude <= amplitudeStep) {
            return zeroAmplitudeDb;
        }
        double amplitudeDb = toDecibel(amplitude);

        assert !Double.isInfinite(amplitudeDb);

        return amplitudeDb;
    }

    private static double toDecibel(double amplitude) {
        return 20 * FastMath.log10(amplitude);
    }

    /**
     * rescale: [DB_THRESHOLD; 0] -> [-1; 0] -> [0; 1]
     */
    public double rescale(double amplitudeDb) {
        return 1 - (amplitudeDb * zeroAmplitudeDbInv);
    }
}