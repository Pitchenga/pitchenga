package com.harmoneye.math.window;

import org.apache.commons.math3.util.FastMath;

public class HammingWindow implements WindowFunction {

    private static final double TWO_PI = 2 * FastMath.PI;

    @Override
    public double value(double x) {
        return 0.54 - 0.46 * FastMath.cos(TWO_PI * x);
    }

}