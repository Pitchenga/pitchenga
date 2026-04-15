package com.harmoneye.math.cqt;

import com.harmoneye.math.window.HammingWindow;
import com.harmoneye.math.window.WindowFunction;
import org.apache.commons.math3.util.FastMath;

public class CqtContext {
    private int octaves = 6;
    private Integer kernelOctaves;
    private double maxFreq = 4186.0090448064;
    private double samplingFreq = 22050;
    private int halftonesPerOctave = 12;
    public static int binsPerHalftone = 5;
    private WindowFunction window = new HammingWindow();
    private double chopThreshold = 0.005;

    private double baseFreq;
    private int binsPerOctave;
    private double binsPerOctaveInv;
    private int totalBins;
    private int kernelBins;
    private int firstKernelBin;
    private double q;
    private double windowIntegral;
    private int signalBlockSize;
    private double normalizationFactor;

    private CqtContext() {
    }

    public static Builder create() {
        return new Builder();
    }

    private void update() {
        CqtCalculator calc = new CqtCalculator(this);
        if (kernelOctaves == null) {
            kernelOctaves = octaves;
        }
        baseFreq = FastMath.pow(2, -octaves) * maxFreq;
        binsPerOctave = halftonesPerOctave * binsPerHalftone;
        binsPerOctaveInv = 1.0 / binsPerOctave;
        totalBins = binsPerOctave * octaves;
        kernelBins = binsPerOctave * kernelOctaves;
        firstKernelBin = totalBins - kernelBins;
        q = 1 / (FastMath.pow(2, binsPerOctaveInv) - 1);
        windowIntegral = calc.windowIntegral(window);

        signalBlockSize = calc.nextPowerOf2(calc.bandWidth(firstKernelBin));
        normalizationFactor = 2 / (signalBlockSize * windowIntegral);

//		System.out.println(this);
    }

    public int getOctaves() {
        return octaves;
    }

    public void setOctaves(int octaves) {
        this.octaves = octaves;
    }

    public int getKernelOctaves() {
        return kernelOctaves;
    }

    public void setKernelOctaves(int kernelOctaves) {
        this.kernelOctaves = kernelOctaves;
    }

    public double getBaseFreq() {
        return baseFreq;
    }

    public void setMaxFreq(double maxFreq) {
        this.maxFreq = maxFreq;
    }

    public double getSamplingFreq() {
        return samplingFreq;
    }

    public void setSamplingFreq(double samplingFreq) {
        this.samplingFreq = samplingFreq;
    }

    public int getHalftonesPerOctave() {
        return halftonesPerOctave;
    }

    public void setHalftonesPerOctave(int halftonesPerOctave) {
        this.halftonesPerOctave = halftonesPerOctave;
    }

    public int getBinsPerHalftone() {
        return binsPerHalftone;
    }

    public void setBinsPerHalftone(int binsPerHalftone) {
        CqtContext.binsPerHalftone = binsPerHalftone;
    }

    public WindowFunction getWindow() {
        return window;
    }

    public void setWindow(WindowFunction window) {
        this.window = window;
    }

    public double getChopThreshold() {
        return chopThreshold;
    }

    public void setChopThreshold(double chopThreshold) {
        this.chopThreshold = chopThreshold;
    }

    public double getMaxFreq() {
        return maxFreq;
    }

    public int getBinsPerOctave() {
        return binsPerOctave;
    }

    public double getBinsPerOctaveInv() {
        return binsPerOctaveInv;
    }

    public int getTotalBins() {
        return totalBins;
    }

    public int getKernelBins() {
        return kernelBins;
    }

    public int getFirstKernelBin() {
        return firstKernelBin;
    }

    public double getQ() {
        return q;
    }

    public double getWindowIntegral() {
        return windowIntegral;
    }

    public int getSignalBlockSize() {
        return signalBlockSize;
    }

    public double getNormalizationFactor() {
        return normalizationFactor;
    }

    @Override
    public String toString() {
        return "CqtContext [octaves=" + octaves + ", kernelOctaves="
                + kernelOctaves + ", maxFreq=" + maxFreq + ", samplingFreq="
                + samplingFreq + ", halftonesPerOctave=" + halftonesPerOctave
                + ", binsPerHalftone=" + binsPerHalftone + ", window=" + window
                + ", chopThreshold=" + chopThreshold + ", baseFreq=" + baseFreq
                + ", binsPerOctave=" + binsPerOctave + ", binsPerOctaveInv="
                + binsPerOctaveInv + ", totalBins=" + totalBins + ", kernelBins="
                + kernelBins + ", firstKernelBin=" + firstKernelBin + ", q=" + q
                + ", windowIntegral=" + windowIntegral + ", signalBlockSize="
                + signalBlockSize + ", normalizationFactor=" + normalizationFactor + "]";
    }

    public static class Builder {

        private final CqtContext ctx = new CqtContext();

        public CqtContext build() {
            ctx.update();
            return ctx;
        }

        public Builder octaves(int octaves) {
            ctx.setOctaves(octaves);
            return this;
        }

        public Builder kernelOctaves(int kernelOctaves) {
            ctx.setKernelOctaves(kernelOctaves);
            return this;
        }

        public Builder maxFreq(double maxFreq) {
            ctx.setMaxFreq(maxFreq);
            return this;
        }

        public Builder samplingFreq(double samplingFreq) {
            ctx.setSamplingFreq(samplingFreq);
            return this;
        }

        public Builder halftonesPerOctave(int halftonesPerOctave) {
            ctx.setHalftonesPerOctave(halftonesPerOctave);
            return this;
        }

        public Builder binsPerHalftone(int binsPerHalftone) {
            ctx.setBinsPerHalftone(binsPerHalftone);
            return this;
        }

        public Builder window(WindowFunction window) {
            ctx.setWindow(window);
            return this;
        }

        public Builder chopThreshold(int chopThreshold) {
            ctx.setChopThreshold(chopThreshold);
            return this;
        }
    }
}