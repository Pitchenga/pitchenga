package com.harmoneye.math.filter;

import java.util.Arrays;

public class ButterworthFilter implements Filter {
    /*
     * Digital filter designed by mkfilter/mkshape/gencode by A.J. Fisher
     *
     * Command line: /www/usr/fisher/helpers/mkfilter -Bu -Lp -o 6 -a
     * 2.5000000000e-01 0.0000000000e+00 -l
     */

    private static final int NZEROS = 6;
    private static final int NPOLES = 6;
    private static final double GAIN_INV = 1.0f / 3.379723001e+01f;

    private static final double[] INPUT_COEFFS = new double[]{1, 6, 15, 20, 15,
            6, 1};
    private static final double[] OUTPUT_COEFFS = new double[]{-0.0017509260f,
            0, -0.1141994251f, 0, -0.7776959619f, 0, 0};

    private final double[] inputSamples = new double[NZEROS + 1];
    private final double[] outputSamples = new double[NPOLES + 1];

    private double[] filteredSignal;

    private boolean streamingEnabled;

    public static ButterworthFilter newStreamingFilter() {
        ButterworthFilter filter = new ButterworthFilter();
        filter.streamingEnabled = true;
        return filter;
    }

    /**
     * Low-pass filters the time-domain signal in place by a 6th order Butterworth
     * filter. The cut-off frequency is half of the Nyquist frequency (ie. 0.25 x
     * sampling frequency.)
     * <p>
     * This function is not thread-safe.
     */
    public double[] filter(double[] signal) {
        if (filteredSignal == null || filteredSignal.length != signal.length) {
            filteredSignal = new double[signal.length];
        }

        if (!streamingEnabled) {
            clean(inputSamples);
            clean(outputSamples);
        }

        for (int i = 0; i < signal.length; i++) {
            // TODO: instead of shifting use a "pointer" - like in a ring buffer
            shift(inputSamples);
            inputSamples[6] = signal[i] * GAIN_INV;
            shift(outputSamples);
            outputSamples[6] = dot(INPUT_COEFFS, inputSamples)
                    + dot(OUTPUT_COEFFS, outputSamples);
            filteredSignal[i] = outputSamples[6];
        }

        return filteredSignal;
    }

    private void shift(double[] values) {
        if (values.length - 1 >= 0) {
            System.arraycopy(values, 1, values, 0, values.length - 1);
        }
    }

    private double dot(double[] a, double[] b) {
        double sum = 0;
        for (int i = 0; i < a.length; i++) {
            sum += a[i] * b[i];
        }
        return sum;
    }

    private void clean(double[] values) {
        Arrays.fill(values, 0);
    }
}