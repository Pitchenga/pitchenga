package com.harmoneye.analysis;

import com.harmoneye.math.cqt.CqtContext;

public class AnalyzedFrame {

    private final double[] amplitudeSpectrumDbBins;
    private final double[] octaveBins;
    private final CqtContext cqtContext;
    private final double[] pitchClasses;

    public AnalyzedFrame(CqtContext cqtContext, double[] amplitudeSpectrumDbBins, double[] octaveBins,
                         double[] pitchClasses) {
        this.amplitudeSpectrumDbBins = amplitudeSpectrumDbBins;
        this.octaveBins = octaveBins;
        this.cqtContext = cqtContext;
        this.pitchClasses = pitchClasses;
    }

    public double[] getAmplitudeSpectrumDbBins() {
        return amplitudeSpectrumDbBins;
    }

    public double[] getOctaveBins() {
        return octaveBins;
    }

    public CqtContext getCqtContext() {
        return cqtContext;
    }

    public double[] getPitchClasses() {
        return pitchClasses;
    }
}