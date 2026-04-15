package com.harmoneye.math.cqt;

import com.harmoneye.math.matrix.ComplexVector;

/**
 * Represents a Constant-Q Transform.
 * <p>
 * A Constant-Q Transform is similar to DFT (Discrete Fourier Transform) except
 * that the frequency bins have variable bandwidths. More precisely the
 * bandwidth of each bin directly depends on its center frequency. In other
 * words the ratio between the bandwidth and the center frequency (called Q) is
 * constant.
 * <p>
 * In contrast the traditional DFT has constant bandwidth of each bins and
 * variable Q.
 * <p>
 * Center frequencies of DFT bins grow linearly while for CQT they grow
 * exponentially. Conversely bin indexes depend on their center frequencies
 * logarithmically. This can be exploited in music since it corresponds to human
 * hearing.
 */
public interface Cqt {

    /**
     * Transforms a time-domain signal frame to a spectrum with frequency bins
     * spaced with constant Q.
     */
    ComplexVector transform(double[] signal);

}