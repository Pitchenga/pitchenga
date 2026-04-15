package com.harmoneye.math.matrix;

public class ComplexVector {
    // interleaved elements
    // elements[2*i] - real
    // elements[2*i + 1] - imaginary
    private final double[] elements;
    private final int size;

    public ComplexVector(int size) {
        this.size = size;
        this.elements = new double[2 * size];
    }

    public ComplexVector(double[] elements) {
        if (elements.length % 2 != 0) {
            throw new IllegalArgumentException("element length must be even");
        }
        this.size = elements.length / 2;
        this.elements = elements;
    }

    public double[] getElements() {
        return elements;
    }

    public int size() {
        return size;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("[");
        for (int i = 0; i < elements.length; i++) {
            if (i > 0) {
                sb.append(", ");
            }
            sb.append(elements[i]);
        }
        sb.append("]");
        return sb.toString();
    }
}