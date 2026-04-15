package com.harmoneye.app;

import com.harmoneye.analysis.AnalyzedFrame;
import com.harmoneye.analysis.MusicAnalyzer;
import com.harmoneye.viz.OpenGlCircularVisualizer;
import com.harmoneye.viz.OpenGlLinearVisualizer;
import com.harmoneye.viz.SwingVisualizer;
import com.pitchenga.Pitchenga;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.KeyEvent;
import java.net.URI;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.prefs.Preferences;

public class AbstractHarmonEyeApp {

    //    private static final int TIME_PERIOD_MILLIS = 64;
//    private static final int TIME_PERIOD_MILLIS = 32;
    private static final int TIME_PERIOD_MILLIS = 16;
    //    private static final int TIME_PERIOD_MILLIS = 8;
    private static final String WINDOW_TITLE = "Pitchenga";

    protected static final float AUDIO_SAMPLE_RATE = 48000.0f;
    protected static final int AUDIO_BITS_PER_SAMPLE = 16;

    protected MusicAnalyzer soundAnalyzer;

    private final JFrame frame;

    private final CircleOfFifthsEnabledAction circleOfFifthsEnabledAction;
    private final PauseAction pauseAction;
    private JMenuItem pauseMenuItem;
    private final AccumulationEnabledAction accumulationEnabledAction;

    private final SwingVisualizer<AnalyzedFrame> visualizer;
    private final AtomicBoolean initialized = new AtomicBoolean();

    private Timer updateTimer;

    public AbstractHarmonEyeApp(Pitchenga pitchenga) {
        visualizer = new com.harmoneye.viz.OpenGlCircularVisualizer();
//        visualizer = new com.harmoneye.viz.OpenGlLinearVisualizer();

        soundAnalyzer = new MusicAnalyzer(visualizer, AUDIO_SAMPLE_RATE, AUDIO_BITS_PER_SAMPLE, pitchenga);

        circleOfFifthsEnabledAction = new CircleOfFifthsEnabledAction("Circle of fifths", null, "", KeyEvent.VK_F);
        pauseAction = new PauseAction("Pause", null, "", KeyEvent.VK_P);
        accumulationEnabledAction = new AccumulationEnabledAction("Accumulate", null, "", KeyEvent.VK_A);

        frame = createFrame(pitchenga);

        frame.setVisible(true);
    }

    private JFrame createFrame(Pitchenga frame) {
//        JFrame frame = new JFrame(WINDOW_TITLE);
//        frame.add(visualizer.getComponent());
//        frame.getMainPanel().add(visualizer.getComponent(), BorderLayout.NORTH);
        frame.getMainPanel().add(visualizer.getComponent(), BorderLayout.CENTER);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

//        Preferences prefs = Preferences.userNodeForPackage(this.getClass());
//        int x = prefs.getInt("x", -1);
//        int y = prefs.getInt("y", -1);
//        int width = prefs.getInt("width", -1);
//        int height = prefs.getInt("height", -1);
//
//        if (width != -1 && height != -1) {
//            frame.setSize(width, height);
//        } else if (visualizer instanceof OpenGlCircularVisualizer) {
//            if (OpenGlCircularVisualizer.DRAW_SNOWFLAKE) {
//                frame.setSize(1080, 1110);
//            } else {
////                frame.setSize(700, 773);
////                frame.setSize(700, 728);
////                frame.setSize(512, 540);
//                frame.setSize(572, 600);
//            }
//        } else if (visualizer instanceof OpenGlLinearVisualizer) {
//            frame.setSize(1900, 620);
//        }
//        frame.setLocationRelativeTo(null);
//        if (x != -1 && y != -1) {
////        frame.setLocation(0, 0);
////        frame.setLocation(0, 700);
////        frame.setLocation(424, 25);
////        frame.setLocation(86, 521);
////        frame.setLocation(-948, 25);
////        frame.setLocation(303, 25);
////        frame.setLocation(108, 30);
//            frame.setLocation(x, y);
////        frame.setLocation(0, 158);
////        frame.setLocation(0, 220);
//        }
//
//        frame.addComponentListener(new ComponentAdapter() {
//            @Override
//            public void componentResized(ComponentEvent e) {
//                saveWindowState(frame);
//            }
//
//            @Override
//            public void componentMoved(ComponentEvent e) {
//                saveWindowState(frame);
//            }
//        });

        //fixme: Recover menubar?
        //fixme: Without menubar nothing works :)
        frame.setJMenuBar(createMenuBar());
        return frame;
    }

//    private void saveWindowState(JFrame frame) {
//        Preferences prefs = Preferences.userNodeForPackage(this.getClass());
//        prefs.putInt("x", frame.getX());
//        prefs.putInt("y", frame.getY());
//        prefs.putInt("width", frame.getWidth());
//        prefs.putInt("height", frame.getHeight());
//    }

    private JMenuBar createMenuBar() {
        JMenuBar menuBar = new JMenuBar();

        menuBar.add(createVisualizationMenu());
        menuBar.add(createWindowMenu());
        menuBar.add(createHelpMenu());
        menuBar.setVisible(false);
        return menuBar;
    }

    private JMenu createVisualizationMenu() {
        JMenu menu = new JMenu("Visualization");

        pauseMenuItem = new JMenuItem(pauseAction);
        pauseMenuItem.setAccelerator(KeyStroke.getKeyStroke(' '));
        menu.add(pauseMenuItem);

        JCheckBoxMenuItem circleOfFifthsEnabledMenuItem = new JCheckBoxMenuItem(circleOfFifthsEnabledAction);
        circleOfFifthsEnabledMenuItem.setAccelerator(KeyStroke.getKeyStroke('f'));
        menu.add(circleOfFifthsEnabledMenuItem);

        JCheckBoxMenuItem accumulationEnabledMenuItem = new JCheckBoxMenuItem(accumulationEnabledAction);
        accumulationEnabledMenuItem.setAccelerator(KeyStroke.getKeyStroke('a'));
        menu.add(accumulationEnabledMenuItem);
        return menu;
    }

    private JMenu createWindowMenu() {
        JMenu menu = new JMenu("Window");

        final JCheckBoxMenuItem alwaysOnTopMenuItem = new JCheckBoxMenuItem();
        alwaysOnTopMenuItem.setAction(new AbstractAction("Always on top") {
            private static final long serialVersionUID = 1L;

            @Override
            public void actionPerformed(ActionEvent e) {
                frame.setAlwaysOnTop(alwaysOnTopMenuItem.getState());
            }
        });
        alwaysOnTopMenuItem.setAccelerator(KeyStroke.getKeyStroke('t'));
        menu.add(alwaysOnTopMenuItem);
        return menu;
    }

    private JMenu createHelpMenu() {
        JMenu menu = new JMenu("Help");

        final JCheckBoxMenuItem helpMenuItem = new JCheckBoxMenuItem();
        helpMenuItem.setAction(new AbstractAction("Open website") {
            private static final long serialVersionUID = 1L;

            @Override
            public void actionPerformed(ActionEvent e) {
                WebHelper.openWebpage(WebHelper.HELP_URL);
            }
        });
        menu.add(helpMenuItem);
        return menu;
    }

    public void start() {
        if (!initialized.get()) {
            return;
        }

        updateTimer = new Timer("update timer");
        TimerTask updateTask = new TimerTask() {
            @Override
            public void run() {
                if (!Pitchenga.isPlaying()) {
                    soundAnalyzer.updateSignal();
                }
            }
        };
        updateTimer.scheduleAtFixedRate(updateTask, 200, TIME_PERIOD_MILLIS);
        pauseMenuItem.setText("Pause");
        frame.setTitle(WINDOW_TITLE);
    }

    public void stop() {
        if (!initialized.get()) {
            return;
        }

        updateTimer.cancel();
        updateTimer = null;
        pauseMenuItem.setText("Play");
        frame.setTitle("= " + WINDOW_TITLE + " =");
    }

    public void init() {
        soundAnalyzer.init();
        initialized.set(true);
    }

    private void toggle() {
        if (updateTimer != null) {
            stop();
        } else {
            start();
        }
    }

    public JFrame getFrame() {
        return frame;
    }

    private class CircleOfFifthsEnabledAction extends AbstractAction {
        private static final long serialVersionUID = 1L;

        boolean fifthsEnabled = false;

        public CircleOfFifthsEnabledAction(String text, ImageIcon icon, String desc, Integer mnemonic) {
            super(text, icon);
            putValue(SHORT_DESCRIPTION, desc);
            putValue(MNEMONIC_KEY, mnemonic);
        }

        public void actionPerformed(ActionEvent e) {
            fifthsEnabled = !fifthsEnabled;
            visualizer.setPitchStep(fifthsEnabled ? 7 : 1);
            //visualizer.getPanel().repaint();
        }
    }

    public class PauseAction extends AbstractAction {
        private static final long serialVersionUID = 1L;

        public PauseAction(String text, ImageIcon icon, String desc, Integer mnemonic) {
            super(text, icon);
            putValue(SHORT_DESCRIPTION, desc);
            putValue(MNEMONIC_KEY, mnemonic);
        }

        public void actionPerformed(ActionEvent e) {
            toggle();
        }
    }

    public class AccumulationEnabledAction extends AbstractAction {
        private static final long serialVersionUID = 1L;

        public AccumulationEnabledAction(String text, ImageIcon icon, String desc, Integer mnemonic) {
            super(text, icon);
            putValue(SHORT_DESCRIPTION, desc);
            putValue(MNEMONIC_KEY, mnemonic);
        }

        public void actionPerformed(ActionEvent e) {
            soundAnalyzer.toggleAccumulatorEnabled();
        }
    }

    private static class WebHelper {

        // TODO: the help page!
        public static final String HELP_URL = "http://harmoneye.com/?utm_campaign=help&utm_medium=macosxapp";

        public static void openWebpage(String uri) {
            if (!Desktop.isDesktopSupported()) {
                return;
            }
            Desktop desktop = Desktop.getDesktop();
            if (!desktop.isSupported(Desktop.Action.BROWSE)) {
                return;
            }
            try {
                desktop.browse(new URI(uri));
            } catch (Exception ex) {
                throw new RuntimeException(ex);
            }
        }
    }

}
