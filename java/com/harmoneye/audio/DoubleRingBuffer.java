package com.harmoneye.audio;

/**
 * Lock-free ring buffer.
 * <p>
 * Two concurrent threads might overwrite each other's data.
 */
public class DoubleRingBuffer {

    private final int bufferSize;
    private final double[] buffer;
    private volatile int endIndex = 0;

    public DoubleRingBuffer(int bufferSize) {
        this.bufferSize = bufferSize;
        this.buffer = new double[bufferSize];
    }

    /**
     * Appends the given data to the end of the buffer.
     * <p>
     * Older data get overwritten.
     *
     * @param samples
     */
    public void write(double[] samples) {
        int startIndex = endIndex;
        int endIndex = incrementIndex(this.endIndex, samples.length);
        this.endIndex = endIndex;

        if (startIndex < endIndex) {
            System.arraycopy(samples, 0, buffer, startIndex, samples.length);
        } else {
            int read = bufferSize - startIndex;
            System.arraycopy(samples, 0, buffer, startIndex, read);
            System.arraycopy(samples, read - 1, buffer, 0, samples.length - read);
        }
    }

    /**
     * Reads last {@code length} elements appended to the buffer (from the end)
     * into the provided {@code result} array.
     *
     * @param length can be lower or equal to result.length
     * @param result
     */
    public void readLast(int length, double[] result) {
        int startIndex = incrementIndex(endIndex, -length);
        int read = bufferSize - startIndex;
        System.arraycopy(buffer, startIndex, result, 0, read);
        if (read < length) {
            System.arraycopy(buffer, 0, result, read - 1, length - read);
        }
    }

    private int incrementIndex(int value, int increment) {
        int result = value + increment;
        if (result < 0) {
            return result + bufferSize;
        } else if (result > bufferSize - 1) {
            return result - bufferSize;
        } else {
            return result;
        }
    }
}