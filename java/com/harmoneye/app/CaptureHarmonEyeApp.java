package com.harmoneye.app;

import com.harmoneye.analysis.AnalyzedFrame;
import com.harmoneye.audio.Capture;
import com.harmoneye.viz.Visualizer;
import com.pitchenga.Pitchenga;

public class CaptureHarmonEyeApp extends AbstractHarmonEyeApp {

    private final Capture capture;

    public CaptureHarmonEyeApp(Pitchenga pitchenga) {
        super(pitchenga);
        capture = new Capture(soundAnalyzer, AUDIO_SAMPLE_RATE, AUDIO_BITS_PER_SAMPLE);
    }

    public void start() {
        super.start();
        capture.start();
    }

    public void stop() {
        super.stop();
        capture.stop();
    }
}