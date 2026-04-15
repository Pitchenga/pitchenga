package com.harmoneye.audio;

import static com.pitchenga.Pitchenga.debug;

import java.util.concurrent.atomic.AtomicBoolean;

import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.DataLine;
import javax.sound.sampled.TargetDataLine;

public class Capture implements Runnable {

    // TODO: Automatically find out minimum usable buffer size.
    // If its too small, buffer underruns make weird artifacts!!!

    private static final int DEFAULT_READ_BUFFER_SIZE_SAMPLES = 1024;
//	private static final int DEFAULT_READ_BUFFER_SIZE_SAMPLES = 512;

    private Thread thread;
    private final AtomicBoolean isRunning = new AtomicBoolean();

    private final int readBufferSizeInSamples;
    private final AudioFormat format;
    private final int bufferSize;

    private final SoundConsumer soundConsumer;

    public Capture(SoundConsumer soundConsumer, float sampleRate, int sampleSizeBits) {
        this.soundConsumer = soundConsumer;

        AudioFormat.Encoding encoding = AudioFormat.Encoding.PCM_SIGNED;
        int channelCount = 2;
        int sampleSizeBytes = sampleSizeBits / 8;
        int frameSizeBytes = channelCount * sampleSizeBytes;
        boolean bigEndian = false;

        format = new AudioFormat(encoding, sampleRate, sampleSizeBits, channelCount, frameSizeBytes, sampleRate,
                bigEndian);

        readBufferSizeInSamples = DEFAULT_READ_BUFFER_SIZE_SAMPLES;
        bufferSize = frameSizeBytes * readBufferSizeInSamples;
        debug("buffer size: " + bufferSize + " B, " + readBufferSizeInSamples + " samples");
    }

    public void start() {
        thread = new Thread(this);
        thread.setName("Capture");
        thread.start();
        isRunning.set(true);
    }

    public void stop() {
        isRunning.set(false);
    }

    public void run() {
        try {
            capture();
        } catch (Exception e) {
            e.printStackTrace(System.err);
            thread = null;
        }
    }

    private void capture() throws Exception {
        byte[] data = new byte[bufferSize];
        double[] amplitudes = new double[readBufferSizeInSamples];

        //fixme: Re-init on combo change
//        Mixer.Info mixerInfo = Pitchenga.INSTANCE.getSelectedMixer();
//        if (mixerInfo == null || mixerInfo == Setup.NO_AUDIO_INPUT) {
//            System.out.println("No audio input selected, play using keyboard or mouse");
//            System.out.println("To play using a musical instrument please select an audio input");
//            return;
//        }
//        Mixer mixer = AudioSystem.getMixer(mixerInfo);
//        float sampleRate = 44100;
//        AudioFormat format = new AudioFormat(sampleRate, 16, 1, true, false);
//        DataLine.Info dataLineInfo = new DataLine.Info(TargetDataLine.class, format);
//        TargetDataLine line = (TargetDataLine) mixer.getLine(dataLineInfo);

        debug("Capture format=" + format);
        DataLine.Info info = new DataLine.Info(TargetDataLine.class, format);
        debug("Capture dataLineInfo=" + info);
        TargetDataLine line = (TargetDataLine) AudioSystem.getLine(info);
        line.open(format, bufferSize);
        line.start();

        while (isRunning.get()) {
            int readBytesCount = line.read(data, 0, bufferSize);
            if (readBytesCount == -1) {
                break;
            }
            ByteConverter.littleEndianStereoBytesToMonoDoubles(data, amplitudes);
            soundConsumer.consume(amplitudes);
        }

        // we reached the end of the stream.
        // stop and close the line.
        line.stop();
        line.close();
    }
}