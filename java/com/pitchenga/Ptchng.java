package com.pitchenga;

import com.harmoneye.app.AbstractHarmonEyeApp;
import com.harmoneye.app.CaptureHarmonEyeApp;
import com.harmoneye.viz.OpenGlCircularVisualizer;
import com.pitchenga.domain.Buzzer;
import com.pitchenga.domain.Hinter;
import com.pitchenga.domain.Instrument;
import com.pitchenga.domain.Pacer;
import com.pitchenga.domain.Riddler;
import com.pitchenga.domain.Ringer;

import java.lang.reflect.InvocationTargetException;
import java.util.Arrays;

import javax.swing.SwingUtilities;
import javax.swing.SwingWorker;

import be.tarsos.dsp.pitch.PitchProcessor;

@SuppressWarnings("unused")
public class Ptchng extends Setup {


    public Ptchng() {

        defaultPenaltyFactor = 3;
        defaultPenaltyFactor = 0;

        defaultOctaves = new Integer[]{2, 3, 4, 5, 6};


        // MPM is smoother - no octave jitters
        defaultPitchAlgo = PitchProcessor.PitchEstimationAlgorithm.MPM;
//        defaultPitchAlgo = PitchProcessor.PitchEstimationAlgorithm.AMDF;
//        defaultPitchAlgo = PitchProcessor.PitchEstimationAlgorithm.DYNAMIC_WAVELET;
//        defaultPitchAlgo = PitchProcessor.PitchEstimationAlgorithm.YIN;

//        riddleInstrument = Instrument.ELECTRIC_BASS_FINGER;
//        riddleInstruments = new int[] { Instrument.ELECTRIC_PIANO_2 };
        riddleInstruments = new int[]{
                Instrument.ELECTRIC_BASS_FINGER,
                Instrument.ACOUSTIC_GRAND_PIANO,
                Instrument.ELECTRIC_PIANO_1,
                Instrument.CHURCH_ORGAN,
                Instrument.TENOR_SAX
        };
        voiceHints = true;

//        riddleInstrument = Instrument.TENOR_SAX;
//        keyboardInstrument = Instrument.ELECTRIC_GUITAR_CLEAN;
        keyboardInstrument = Instrument.ELECTRIC_PIANO_1;
//        keyboardInstrument = Instrument.ELECTRIC_PIANO_2;
//        ringInstrument = Instrument.ACOUSTIC_GRAND_PIANO;

        //fixme: Move to riddle
        seriesLength = 4;
        repeats = 3;

        defaultHinter = Hinter.Series;
        //fixme: Delay hinter proportionally to the tempo

        defaultPacer = Pacer.Answer;
        defaultPacer = Pacer.Tempo100;
        defaultPacer = Pacer.Tempo70;
        defaultPacer = Pacer.Tempo80;

        defaultRiddler = Riddler.Rec;
        defaultRiddler = Riddler.Bass;
        defaultRiddler = Riddler.BassScales;

        defaultRinger = Ringer.None;

        defaultBuzzer = Buzzer.Tone;

        defaultAudioInput = null;

        mainFrameVisible = true;
        fullScreenWhenPlaying = false;
        fullScreenWhenPlaying = true;

        if (OpenGlCircularVisualizer.RECORD_VIDEO) {
            defaultRiddler = Riddler.FuguesOrdered;
            voiceHints = false;
            defaultHinter = Hinter.Always;
            defaultPacer = Pacer.Tempo30;
            fullScreenWhenPlaying = false;
        }
        if (defaultRiddler.equals(Riddler.Rec)) {
            defaultPacer = Pacer.Tempo60;
            defaultHinter = Hinter.Never;
            fullScreenWhenPlaying = false;
            repeats = 20;
            seriesLength = 1;
            voiceHints = false;
        }

    }

    public static void main(String[] args) throws InvocationTargetException, InterruptedException {
        System.out.println(Arrays.toString(args));
//        System.setProperty("com.pitchenga.debug", "true");
        System.setProperty("com.pitchenga.setup.class", "com.pitchenga.Ptchng");

        SwingUtilities.invokeAndWait(() -> {
//            String defaultInput = System.getProperty("com.pitchenga.default.input");
//            if (defaultInput == null) {
//            Pitchenga secondary = new Pitchenga(false, null);
//            System.setProperty("com.pitchenga.default.input", "NO_AUDIO_INPUT");
//            System.setProperty("com.pitchenga.default.input", "Sonic Port VX");
//                System.setProperty("com.pitchenga.default.input", "Microphone (High Definition Aud");
//            System.setProperty("com.pitchenga.default.input", "STUDIO-CAPTURE");
//            System.setProperty("com.pitchenga.default.input", "HD Pro Webcam C920");
//            }
//            Pitchenga primary = new Pitchenga(true, secondary);
            Pitchenga primary = new Pitchenga(true, null);
            AbstractHarmonEyeApp app = harmonEye(primary);
            primary.setEye(app.getFrame());

//            secondary.requestFocus();
//            Rectangle screenSize = GraphicsEnvironment.getLocalGraphicsEnvironment().getMaximumWindowBounds();
//            int side = Math.min(screenSize.height, screenSize.width);
//            primary.setSize(side, side);
//            primary.setLocation(screenSize.width / 2 - primary.getSize().width / 2, screenSize.height / 2 - primary.getSize().height / 2);
//            primary.setLocation(0, 0);
//            primary.setSize(screenSize.width, screenSize.height);
//            primary.setExtendedState(primary.getExtendedState() | JFrame.MAXIMIZED_BOTH);
        });
    }

    public static AbstractHarmonEyeApp harmonEye(Pitchenga pitchenga) {
        CaptureHarmonEyeApp captureHarmonEyeApp = new CaptureHarmonEyeApp(pitchenga);
        class Initializer extends SwingWorker<String, Object> {
            @Override
            public String doInBackground() {
                captureHarmonEyeApp.init();
                captureHarmonEyeApp.start();
                return null;
            }
        }
        new Initializer().execute();
        return captureHarmonEyeApp;
    }

}