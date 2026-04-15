package com.harmoneye.analysis;

public class ScalarExpSmoother {

    double data;
    double currentWeight;
    double previousWeight;

    public ScalarExpSmoother(double currentWeight) {
        this.currentWeight = currentWeight;
        previousWeight = 1 - currentWeight;
    }

    public double smooth(double currentFrame) {
        data = previousWeight * data + currentWeight * currentFrame;
        return data;
    }

}