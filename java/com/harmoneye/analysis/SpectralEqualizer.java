package com.harmoneye.analysis;

import com.harmoneye.math.stats.Median;
import org.apache.commons.math3.util.FastMath;

public class SpectralEqualizer {

    private static final double MEDIAN_WEIGHT = 0.75;

    private final int size;
    private final int windowSize;
    private final double[] filteredValues;
    private final double[] window;

    private final Median medianFilter;

    public SpectralEqualizer(int size, int windowSize) {
        this.size = size;
        this.windowSize = windowSize;

        medianFilter = new Median(windowSize);

        filteredValues = new double[size];
        window = new double[windowSize];
    }

    public double[] filter(double[] values) {
        for (int i = 0; i < size; i++) {
            for (int winIndex = 0; winIndex < windowSize; winIndex++) {
                int index = (i + winIndex + size) % size;
                window[winIndex] = values[index];
            }
            double median = medianFilter.evaluate(window);
            filteredValues[i] = values[i] - MEDIAN_WEIGHT * median;
        }

        normalizeViaMax(values, filteredValues);

        return filteredValues;
    }

    private void normalizeViaMax(double[] origBins, double[] newBins) {
        // TODO: this kind of normalization is bad and unstable
        // TODO: use org.apache.commons.math3.stat.descriptive.rank.Max

        double newMax = 0;
        double origMax = 0;
        for (int i = 0; i < newBins.length; i++) {
            newMax = FastMath.max(newMax, newBins[i]);
            origMax = FastMath.max(origMax, origBins[i]);
        }

        if (newMax > 0) {
            double factor = origMax < 1 ? origMax / newMax : 1 / origMax;
            for (int i = 0; i < newBins.length; i++) {
                newBins[i] *= factor;
            }
        }
    }

}