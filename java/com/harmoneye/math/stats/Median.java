package com.harmoneye.math.stats;

import java.util.Arrays;

// This is a simple O(N*log(N)) median algorithm.
// TODO: implement the O(N) one.
public class Median {
    private final boolean isSizeEven;
    private final int midIndex;

    public Median(int size) {
        isSizeEven = size % 2 == 0;
        midIndex = size / 2;
    }

    public double evaluate(double[] values) {
        if (isSizeEven) {
            return 0.5 * (values[midIndex] + values[midIndex - 1]);
        } else {
            return values[midIndex];
        }
    }
}