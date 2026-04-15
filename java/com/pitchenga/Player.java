package com.pitchenga;

import org.apache.commons.math3.util.FastMath;

import javax.sound.sampled.*;
import java.io.IOException;
import java.net.URL;

public class Player {

    private final Clip clip;

//    private static final int AUDIO_BITS_PER_SAMPLE = 16;
//    int sampleSizeBytes = AUDIO_BITS_PER_SAMPLE / 8;
//    private final int readBufferSizeInSamples = 1024;
//    private final int bufferSize = sampleSizeBytes * readBufferSizeInSamples;
//    private final byte[] data = new byte[bufferSize];
//    private final double[] amplitudes = new double[readBufferSizeInSamples];

    public Player(URL resource) {
        try {
            AudioInputStream audioInputStream = AudioSystem.getAudioInputStream(resource);
            this.clip = AudioSystem.getClip();
            clip.open(audioInputStream);
            float volume = 0.2f;
            FloatControl gainControl = (FloatControl) clip.getControl(FloatControl.Type.MASTER_GAIN);
            gainControl.setValue(20f * (float) FastMath.log10(volume));
        } catch (LineUnavailableException | UnsupportedAudioFileException | IOException e) {
            e.printStackTrace();
            throw new RuntimeException(e);
        }
    }

    public void play() {
        new Thread(() -> {
//            MusicAnalyzer.INSTANCE.consume(amplitudes);
            clip.setFramePosition(0);
            clip.start();
        }).start();
    }

    public void stop() {
//        new Thread(() -> {
        clip.stop();
//        }).start();
    }

}