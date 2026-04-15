package com.harmoneye.math.matrix;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class SparseRCComplexMatrix2D {

    private final int rows;
    private final int columns;

    // elements - interleaved real, imaginary
    // size: 2 * nonZeroElements
    double[] elements;
    // column indexes of elements
    // columnIndexes[k] = j is the column of matrix element A_ij
    // size: nonZeroElements
    int[] columnIndexes;
    // indexes of the elements that start a new row
    // the values are indexes of the whole complex numbers, not their components
    // size: rows
    int[] rowPointers;

    public SparseRCComplexMatrix2D(int rows, int columns, double[] elements,
                                   int[] columnIndexes, int[] rowPointers) {
        this.rows = rows;
        this.columns = columns;
        this.elements = elements;
        this.columnIndexes = columnIndexes;
        this.rowPointers = rowPointers;
    }

    public double[] operate(double[] vector) {
        return operate(vector, new double[2 * rows]);
    }

    public double[] operate(double[] vector, double[] result) {
        int resultIndex = 0;
        for (int row = 0; row < rows; row++, resultIndex += 2) {
            double sumRe = 0;
            double sumIm = 0;
            int rowStart = rowPointers[row];
            int nextRowStart = rowPointers[row + 1];
            for (int index = rowStart; index < nextRowStart; index++) {
                int col = columnIndexes[index];
                int vectorIndex = 2 * col;
                int elementIndex = 2 * index;
                double mRe = elements[elementIndex];
                double vRe = vector[vectorIndex];
                double mIm = elements[elementIndex + 1];
                double vIm = vector[vectorIndex + 1];
                sumRe += mRe * vRe - mIm * vIm;
                sumIm += mIm * vRe + mRe * vIm;
            }
            result[resultIndex] = sumRe;
            result[resultIndex + 1] = sumIm;
        }
        return result;
    }

    public static class Builder {

        private final int rows;
        private final int columns;
        private final ArrayList<Double> elements = new ArrayList<>();
        private final ArrayList<Integer> columnIndexes = new ArrayList<>();
        private final ArrayList<Integer> rowPointers = new ArrayList<>();

        public Builder(int rows, int columns) {
            this.rows = rows;
            this.columns = columns;
            rowPointers.add(0);
        }

        public void addRow(int row, ComplexVector vector) {
            if (row >= rows) {
                throw new IllegalArgumentException("row (" + row
                        + ") must be smaller than rows: " + rows);
            }
            if (row >= rowPointers.size()) {
                throw new IllegalArgumentException("row " + row
                        + " was already added");
            }
            double[] vecElems = vector.getElements();
            int size = vector.size();
            int nonZeroValues = 0;
            for (int col = 0; col < size; col++) {
                double re = vecElems[2 * col];
                double im = vecElems[2 * col + 1];
                if (re != 0 || im != 0) {
                    nonZeroValues++;
                    elements.add(re);
                    elements.add(im);
                    columnIndexes.add(col);
                }
            }
            int rowPtr = rowPointers.get(rowPointers.size() - 1);
            int nextRowPtr = rowPtr + nonZeroValues;
            rowPointers.add(nextRowPtr);
        }

        public SparseRCComplexMatrix2D build() {
            return new SparseRCComplexMatrix2D(rows, columns,
                    toDoubleArray(elements), toIntArray(columnIndexes),
                    toIntArray(rowPointers));
        }

        private double[] toDoubleArray(List<Double> list) {
            int size = list.size();
            double[] array = new double[size];
            for (int i = 0; i < size; i++) {
                array[i] = list.get(i);
            }
            return array;
        }

        private int[] toIntArray(List<Integer> list) {
            int size = list.size();
            int[] array = new int[size];
            for (int i = 0; i < size; i++) {
                array[i] = list.get(i);
            }
            return array;
        }
    }

    public int getRows() {
        return rows;
    }

    public int getColumns() {
        return columns;
    }

    @Override
    public String toString() {
        return getClass().getSimpleName() + ' ' +
                rows + 'x' + columns + '\n' +
                "elements:" + Arrays.toString(elements) + '\n' +
                "columnIndexes:" + Arrays.toString(columnIndexes) +
                '\n' +
                "rowPointers:" + Arrays.toString(rowPointers) +
                '\n';
    }
}