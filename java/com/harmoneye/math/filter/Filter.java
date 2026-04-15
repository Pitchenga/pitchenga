package com.harmoneye.math.filter;

public interface Filter {
    /**
     * 1D digital filter.
     * <p>
     * The returned array is owned by the filter and might change when the
     * filter() method is called again. It might be mutable, depending on the
     * actual implementation.
     *
     * @param signal input signal
     * @return filtered signal
     */
    double[] filter(double[] signal);
}