package com.harmoneye.math.cqt;

import com.harmoneye.math.matrix.ComplexVector;
import com.harmoneye.math.matrix.SparseRCComplexMatrix2D;
import com.harmoneye.math.matrix.SparseRCComplexMatrix2D.Builder;
import edu.emory.mathcs.jtransforms.fft.DoubleFFT_1D;
import org.apache.commons.lang3.time.StopWatch;

public class FastCqt implements Cqt {

    protected CqtContext ctx;
    protected CqtCalculator calc;
    private final DoubleFFT_1D fft;

    // spectral kernels - already Hermite-conjugated (conjugated and transposed)
    // for the transform
    private SparseRCComplexMatrix2D spectralKernels;

    private ComplexVector cqtSpectrum;
    private ComplexVector dataRI;

    // ScalarExpSmoother acc = new ScalarExpSmoother(0.01);

    public FastCqt(CqtContext ctx) {
        this.ctx = ctx;
        this.calc = new CqtCalculator(ctx);
        this.fft = new DoubleFFT_1D(ctx.getSignalBlockSize());
    }

    public void init() {
        if (spectralKernels == null) {
            computeSpectralKernels();
        }

        int signalBlockSize = ctx.getSignalBlockSize();
        dataRI = new ComplexVector(signalBlockSize);
        cqtSpectrum = new ComplexVector(spectralKernels.getRows());
    }

    // signal must to be as long as ctx.signalBlockLength()
    @Override
    public ComplexVector transform(double[] signal) {
        // StopWatch sw = new StopWatch();
        // sw.start();

        double[] dataRIElems = dataRI.getElements();
        System.arraycopy(signal, 0, dataRIElems, 0, signal.length);
        fft.realForwardFull(dataRIElems);
        // dftSpectrum = ComplexUtils.complexArrayFromVector(dataRI);

        // Transform the DFT spectrum bins into CQT spectrum bins using the
        // precomputed matrix of kernels (in frequency domain).
        // The normalization factor is already merged into the matrix.
        double[] cqtSpectrumElems = cqtSpectrum.getElements();
        spectralKernels.operate(dataRIElems, cqtSpectrumElems);

        // sw.stop();
        // System.out.println("Computed transformed signal in " +
        // acc.smooth(sw.getNanoTime()) * 0.001 + " us");

        return cqtSpectrum;
    }

    protected void computeSpectralKernels() {
        if (spectralKernels != null) {
            return;
        }
        int rows = ctx.getKernelBins();
        int columns = ctx.getSignalBlockSize();
        System.out.println("rows x columns: " + rows + "x" + columns
                + ", total: " + rows * columns);
        StopWatch sw = new StopWatch();
        sw.start();

        // the matrix is built directly as transposed
        Builder builder = new Builder(rows, columns);
        for (int k = 0; k < rows; k++) {
            builder.addRow(k, calc.conjugatedNormalizedSpectralKernel(k));
        }
        spectralKernels = builder.build();

        sw.stop();
        System.out.println("Computed kernels in " + sw.getTime() + " ms");
    }

    public CqtContext getContext() {
        return ctx;
    }
}