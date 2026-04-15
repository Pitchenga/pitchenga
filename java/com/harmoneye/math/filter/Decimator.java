package com.harmoneye.math.filter;

/**
 * Decimates the signal using a low-pass filter followed by downsampling by
 * factor of two.
 *
 * @see http://en.wikipedia.org/wiki/Downsampling
 */
public class Decimator {

    private final Filter lowPassFilter;

    private double[] decimatedSignal;

    public Decimator(Filter lowPassFilter) {
        this.lowPassFilter = lowPassFilter;
    }

    public static Decimator withDefaultFilter() {
        return new Decimator(new ZeroPhaseFilter(new ButterworthFilter()));
    }

    public double[] decimate(double[] signal) {
        double[] lowPassSignal = lowPassFilter.filter(signal);
        return downsample(lowPassSignal);
    }

    // downsample by factor of two
    private double[] downsample(double[] signal) {
        int halfLength = signal.length / 2;
        if (decimatedSignal == null || decimatedSignal.length != halfLength) {
            decimatedSignal = new double[halfLength];
        }

        for (int to = 0, from = 0; to < decimatedSignal.length; to++, from += 2) {
            decimatedSignal[to] = signal[from];
        }
        return decimatedSignal;
    }
}