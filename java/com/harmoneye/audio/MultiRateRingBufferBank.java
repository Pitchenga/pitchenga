package com.harmoneye.audio;

import com.harmoneye.math.filter.Decimator;

/**
 * A bank of ring buffers with different sample-rates.
 * <p>
 * The first buffer contains the signal with original sample rate, each
 * following buffer contains signal decimated by factor of two compared to the
 * previous buffer.
 * <p>
 * All buffer have the same size in samples (in the particular sample rate),
 * thus the stored signal time differs.
 */
public class MultiRateRingBufferBank {

    /**
     * Ring buffers for multiple octaves (indexed from highest to lowest).
     */
    private final DoubleRingBuffer[] buffers;
    /**
     * Decimators for multiple octaves (indexed from highest to lowest).
     * <p>
     * Decimator K decimates signal from ring buffer K to K - 1. So there's one
     * less decimators than ring buffers.
     */
    private final Decimator[] decimators;

    private final int octaveCount;

    public MultiRateRingBufferBank(int bufferSize, int octaveCount) {
        this.octaveCount = octaveCount;
        int decimatorCount = octaveCount - 1;

        buffers = new DoubleRingBuffer[octaveCount];
        for (int octave = 0; octave < octaveCount; octave++) {
            buffers[octave] = new DoubleRingBuffer(bufferSize);
        }

        decimators = new Decimator[decimatorCount];
        for (int octave = 0; octave < decimatorCount; octave++) {
            decimators[octave] = Decimator.withDefaultFilter();
        }
    }

    /**
     * Appends the given data to the end of the buffer.
     * <p>
     * Older data get overwritten.
     */
    public void write(double[] samples) {
        for (int octave = 0; octave < octaveCount; octave++) {
            buffers[octave].write(samples);
            // decimate samples for the next octave
            if (octave < decimators.length) {
                samples = decimators[octave].decimate(samples);
            }
        }
    }

    /**
     * Reads last {@code length} elements appended to the buffer (from the end)
     * into the provided {@code result} array.
     *
     * @param length can be lower or equal to result.length
     * @param result
     */
    public void readLast(int octave, int length, double[] result) {
        buffers[octave].readLast(length, result);
    }
}