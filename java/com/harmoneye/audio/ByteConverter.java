package com.harmoneye.audio;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class ByteConverter {

    // signed short to [-1; 1]
    private static final double normalizationFactor = 2 / (double) 0xffff;

    public static void bytesToDoubles(byte[] bytes, double[] doubles, ByteOrder byteOrder) {
        assert bytes.length == 2 * doubles.length;
        ByteBuffer byteBuffer = ByteBuffer.wrap(bytes);
        byteBuffer.order(byteOrder);
        for (int i = 0; i < doubles.length && byteBuffer.hasRemaining(); i++) {
            doubles[i] = normalizationFactor * byteBuffer.getShort();
        }
    }

    public static void littleEndianBytesToDoubles(byte[] bytes, double[] doubles) {
        bytesToDoubles(bytes, doubles, ByteOrder.LITTLE_ENDIAN);
    }

    // Mic
    private static final AutomaticGainControl agc1 =
            new AutomaticGainControl(0.0001, 0.4, 2.0);

    // Desktop
    private static final AutomaticGainControl agc2 =
            new AutomaticGainControl(0.0001, 0.4, 2.0);

    public static void littleEndianStereoBytesToMonoDoubles(byte[] bytes, double[] doubles) {
        ByteBuffer byteBuffer = ByteBuffer.wrap(bytes);
        byteBuffer.order(ByteOrder.LITTLE_ENDIAN);

        for (int i = 0; i < doubles.length && byteBuffer.remaining() >= 4; i++) {
            double channel1 = normalizationFactor * byteBuffer.getShort();
            double channel2 = normalizationFactor * byteBuffer.getShort();
            channel1 = agc1.process(channel1);
            channel2 = agc2.process(channel2);

            // Mix down to mono
            doubles[i] = (channel1 + channel2) / 2.0;
        }
    }

    public static void bigEndianBytesToDoubles(byte[] bytes, double[] doubles) {
        bytesToDoubles(bytes, doubles, ByteOrder.BIG_ENDIAN);
    }

}