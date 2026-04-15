package com.harmoneye.math.filter;

/**
 * Forward-and-reverse zero-phase filter decorator.
 * <p>
 * It runs the decorated filter on the signal, then on the reversed signal. The
 * total filter magnitude is squared compared to the original filter and the
 * signal has zero phase distortion.
 *
 * @see http://www.mathworks.com/help/signal/ref/filtfilt.html
 */
public class ZeroPhaseFilter implements Filter {

    private final Filter decoratedFilter;

    public ZeroPhaseFilter(Filter decoratedFilter) {
        this.decoratedFilter = decoratedFilter;
    }

    @Override
    public double[] filter(double[] signal) {
        double[] filteredSignal = decoratedFilter.filter(signal);
        reverse(filteredSignal);
        filteredSignal = decoratedFilter.filter(filteredSignal);
        reverse(filteredSignal);

        return filteredSignal;
    }

    // TODO: The reversing could be eliminated if the decorated filter could be
    // told to run in reverse or the signal would have a reverse iterator.

    private void reverse(double[] signal) {
        // swap the first half of elements with the second half
        double swapped;
        for (int i = 0, j = signal.length - 1; i < j; i++, j--) {
            swapped = signal[i];
            signal[i] = signal[j];
            signal[j] = swapped;
        }
    }

}