package com.harmoneye.audio;

public class AutomaticGainControl {
    private final double alpha;
    private final double targetLevel;
    private final double maxGain;
    private double runningEnvelope = 0.001;

    public AutomaticGainControl(double alpha, double targetLevel, double maxGain) {
        this.alpha = alpha;
        this.targetLevel = targetLevel;
        this.maxGain = maxGain;
    }

    public double process(double sample) {
        // 1. Update the running average
        runningEnvelope = (1.0 - alpha) * runningEnvelope + alpha * Math.abs(sample);

        // 2. Calculate the required gain (+ 0.00001 prevents divide by zero)
        double dynamicGain = targetLevel / (runningEnvelope + 0.00001);

        // 3. Clamp to prevent pure silence from exploding into noise
        dynamicGain = Math.min(dynamicGain, maxGain);

        // 4. Apply gain and Soft-Clip
        return Math.tanh(sample * dynamicGain);
    }
}