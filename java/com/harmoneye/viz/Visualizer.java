package com.harmoneye.viz;

public interface Visualizer<T> {
    void update(T data);

    // TODO: generalize parameters for tuning visualizer
    // eg. with a map of parameters
    void setPitchStep(int i);
}