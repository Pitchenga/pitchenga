package com.harmoneye.analysis;

class MovingAverageAccumulator {
    double[] data;
    int frameCount;

    public MovingAverageAccumulator(int size) {
        data = new double[size];
    }

    public double[] getAverage() {
        return data;
    }

    public double[] add(double[] currentFrame) {
        assert data.length == currentFrame.length;

        frameCount++;

        double weight = 1.0 / frameCount;

        for (int i = 0; i < data.length; i++) {
            data[i] = (1 - weight) * data[i] + weight * currentFrame[i];
        }

        return data;
    }

    public void reset() {
        frameCount = 0;
    }
}