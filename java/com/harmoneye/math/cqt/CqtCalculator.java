package com.harmoneye.math.cqt;

import com.harmoneye.math.matrix.ComplexVector;
import com.harmoneye.math.matrix.DComplex;
import com.harmoneye.math.window.WindowFunction;
import edu.emory.mathcs.jtransforms.fft.DoubleFFT_1D;
import org.apache.commons.math3.analysis.integration.TrapezoidIntegrator;
import org.apache.commons.math3.analysis.integration.UnivariateIntegrator;
import org.apache.commons.math3.util.FastMath;

import java.util.Arrays;

public class CqtCalculator {

    private final CqtContext ctx;

    private DoubleFFT_1D fft;

    public CqtCalculator(CqtContext ctx) {
        this.ctx = ctx;
    }

    public double centerFreq(int binIndex) {
        // (k - binsPerHalftoneHalf) instead of k
        // in order to put the center frequency in the center bin for that halftone
        //fixme: Integer division in floating-point context, but when replacing it with 2.0 it displays sharper than it is.
        return (ctx.getBaseFreq() * FastMath.pow(2, (binIndex - ctx.getBinsPerHalftone() / 2) * ctx.getBinsPerOctaveInv()));
    }

    public int bandWidth(int binIndex) {
        return (int) FastMath.ceil(ctx.getQ() * ctx.getSamplingFreq() / centerFreq(binIndex));
    }

    protected ComplexVector temporalKernel(int kernelBinIndex) {
        int size = bandWidth(kernelBinIndex + ctx.getFirstKernelBin());
        ComplexVector coeffs = new ComplexVector(size);
        double[] coeffsRI = coeffs.getElements();
        double sizeInv = 1.0 / size;
        double factor = 2 * FastMath.PI * ctx.getQ() * sizeInv;
        WindowFunction window = ctx.getWindow();
        for (int i = 0; i < size; i++) {
            double r = window.value(i * sizeInv) * sizeInv;
            double theta = i * factor;
            coeffsRI[2 * i] = r * FastMath.cos(theta);
            coeffsRI[2 * i + 1] = r * FastMath.sin(theta);
        }
        return coeffs;
    }

    public ComplexVector spectralKernel(int k) {
        ComplexVector temporalKernel = padLeft(temporalKernel(k), ctx.getSignalBlockSize());
//		ComplexVector temporalKernel = padCenter(temporalKernel(k), ctx.getSignalBlockSize());
        double[] data = temporalKernel.getElements();
        getFft().complexForward(data);
        ComplexVector spectrum = new ComplexVector(data);

        chop(spectrum, ctx.getChopThreshold());
        return spectrum;
    }

    public ComplexVector conjugatedNormalizedSpectralKernel(int k) {
        ComplexVector spectrum = spectralKernel(k);
        double normalizationFactor = ctx.getNormalizationFactor();

        int size = spectrum.size();
        double[] elements = spectrum.getElements();
        for (int i = 0; i < size; i++) {
            double re = elements[2 * i];
            double im = elements[2 * i + 1];
            if (re != 0 && im != 0) {
                elements[2 * i] = normalizationFactor * re;
                elements[2 * i + 1] = normalizationFactor * -im;
            }
        }

        return spectrum;
    }

    public double windowIntegral(WindowFunction window) {
        UnivariateIntegrator integrator = new TrapezoidIntegrator();
        return integrator.integrate(100, window, 0, 1);
    }

    // http://acius2.blogspot.cz/2007/11/calculating-next-power-of-2.html
    public int nextPowerOf2(int value) {
        value--;
        value |= (value >> 1);
        value |= (value >> 2);
        value |= (value >> 4);
        value |= (value >> 8);
        value |= (value >> 16);
        value++;
        return value;
    }

    public void chop(ComplexVector values, double threshold) {
        int size = values.size();
        double[] elements = values.getElements();
        for (int i = 0; i < size; i++) {
            double re = elements[2 * i];
            double im = elements[2 * i + 1];
            double abs = DComplex.abs(re, im);
            if (abs < threshold) {
                elements[2 * i] = 0;
                elements[2 * i + 1] = 0;
            }
        }
    }

    public ComplexVector conjugate(ComplexVector values) {
        int size = values.size();
        double[] elements = values.getElements();
        for (int i = 0; i < size; i++) {
            elements[2 * i + 1] = -elements[2 * i + 1];
        }
        return values;
    }

    public ComplexVector padLeft(ComplexVector values, int totalSize) {
        if (values.size() == totalSize) {
            return values;
        }
        ComplexVector padded = new ComplexVector(totalSize);
        int dataSize = FastMath.min(values.size(), totalSize);
        int paddingSize = totalSize - dataSize;

        Arrays.fill(padded.getElements(), 0, 2 * paddingSize, 0);
        System.arraycopy(values.getElements(), 0, padded.getElements(), 2 * paddingSize, 2 * dataSize);

        return padded;
    }

    public ComplexVector padCenter(ComplexVector values, int totalSize) {
        if (values.size() == totalSize) {
            return values;
        }
        ComplexVector padded = new ComplexVector(totalSize);
        int dataSize = FastMath.min(values.size(), totalSize);
        int prePaddingSize = (totalSize - dataSize) / 2;

        Arrays.fill(padded.getElements(), 0, 2 * prePaddingSize, 0);
        System.arraycopy(values.getElements(), 0, padded.getElements(), 2 * prePaddingSize, 2 * dataSize);
        Arrays.fill(padded.getElements(), 2 * (prePaddingSize + dataSize), 2 * totalSize - 1, 0);

        return padded;
    }

    public void padRight(double[] in, double[] padded) {
        int dataSize = FastMath.min(in.length, padded.length);

        System.arraycopy(in, 0, padded, 0, dataSize);
        Arrays.fill(padded, dataSize, padded.length, 0);
    }

    public double sum(double[] values) {
        double sum = 0;
        for (int i = 0; i < values.length; i++) {
            sum += values[i];
        }
        return sum;
    }

    public DoubleFFT_1D getFft() {
        if (fft == null) {
            fft = new DoubleFFT_1D(ctx.getSignalBlockSize());
        }
        return fft;
    }

}