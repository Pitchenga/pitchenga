package com.pitchenga;

import com.harmoneye.viz.OpenGlCircularVisualizer;
import com.pitchenga.domain.Button;
import com.pitchenga.domain.Fugue;
import com.pitchenga.domain.Pitch;
import com.pitchenga.domain.Tone;
import com.pitchenga.util.Threads;
import org.apache.commons.math3.util.FastMath;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.util.List;
import java.util.*;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

import static com.harmoneye.viz.OpenGlCircularVisualizer.DARK;
import static com.pitchenga.domain.Pitch.*;
import static com.pitchenga.domain.Tone.*;

public class Display extends JPanel {
    private static final Tone[] TONES = new Tone[]{Fi, Fa, Mi, Me, Re, Ra, Do, Ti, Te, La, Le, So};
    private final Set<Tone> tones = EnumSet.noneOf(Tone.class);
    private final Set<Tone> scaleTones = EnumSet.noneOf(Tone.class);
    private final List<JComponent> allLabelsToScale = new ArrayList<>();
    private final JTextArea textArea = new JTextArea();
    private final JScrollPane textPane = new JScrollPane(textArea);
    private final ScheduledExecutorService asyncExecutor = Executors.newSingleThreadScheduledExecutor(new Threads("pitchenga-display-async"));

    private volatile Pitch pitch;
    private volatile Fugue toneFugue;
    private volatile Color toneColor;
    private volatile Color pitchinessColor;
    private volatile float frequency;
    private volatile Color fillColor;
    private final Piano twoOctavePiano;
    private final Piano oneOctavePiano;
    private final Circle circle;
    private final Frets fretsBase;
    private final Frets fretsFirst;
    private final JComponent[] views;
    private volatile Object currentView;

    public static void main(String[] args) {
        JFrame frame = new JFrame("Display");
        Image image = Toolkit.getDefaultToolkit().getImage(Display.class.getResource("/pitchenga.png"));
        frame.setIconImage(image);

        Display display = new Display();
        int mySide = 700;
        display.setSize(mySide, mySide);
        display.setPreferredSize(new Dimension(mySide, mySide));

        frame.add(display);
        Rectangle screenSize = GraphicsEnvironment.getLocalGraphicsEnvironment().getMaximumWindowBounds();
        frame.setLocation(screenSize.width / 2 - frame.getSize().width / 2, screenSize.height / 2 - frame.getSize().height / 2);
        frame.setVisible(true);
        frame.pack();
    }

    public Display() {
        super();

        //fixme: Concurrently pressed keys are not displayed properly

        this.setLayout(new OverlayLayout(this));

        //fixme: Show/hide text pane on hotkey
        initTextPane();
        JPanel textLayer = new JPanel(new BorderLayout());
        this.add(textLayer);
        textLayer.setOpaque(false);
        textLayer.setBackground(new Color(0, 0, 0, 0.0f));
        textLayer.add(textPane, BorderLayout.EAST);

        Piano oneOctavePiano = new Piano(false);
        this.add(oneOctavePiano);
        this.oneOctavePiano = oneOctavePiano;
        oneOctavePiano.setVisible(false);

        Piano twoOctavePiano = new Piano(true);
        this.add(twoOctavePiano);
        this.twoOctavePiano = twoOctavePiano;
        twoOctavePiano.setVisible(true);

        Circle circle = new Circle();
        this.add(circle);
        this.circle = circle;
        circle.setVisible(false);

        Frets fretsFirst = new Frets(FRETS_FIRST);
        this.add(fretsFirst);
        this.fretsFirst = fretsFirst;

        Frets fretsBase = new Frets(FRETS_BASE);
        fretsBase.setVisible(false);
        this.add(fretsBase);
        this.fretsBase = fretsBase;

        this.views = new JComponent[]{twoOctavePiano, circle, fretsFirst};
        this.currentView = views[0];

        initFontScaling();
    }

    private void initTextPane() {
        textPane.setBorder(null);
        textPane.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
        textPane.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_NEVER);
        textPane.setBackground(new Color(0, 0, 0, 0.0f));
        textPane.getViewport().setBorder(null);
        textPane.getViewport().setBackground(new Color(0, 0, 0, 0.0f));

        //fixme: Copy text to clipboard does not work on mac
        textArea.setFont(Pitchenga.MONOSPACED);
        textArea.setEditable(false);
        textArea.setForeground(Color.LIGHT_GRAY);
        textArea.setBackground(new Color(0, 0, 0, 0.0f));
        textArea.setBorder(null);
        clearText();
    }

    public void clearText() {
        textArea.setText("");
        for (int i = 0; i < 500; i++) { //There must be a better way
//            text("        \n");
            text("   \n");
        }
    }


    public void text(String message) {
        OpenGlCircularVisualizer.text = message;
        asyncExecutor.schedule(() -> SwingUtilities.invokeLater(() -> {
//                    textArea.setVisible(false);
                    OpenGlCircularVisualizer.text = null;
                }),
                15, TimeUnit.SECONDS);
//        text(message, null, null);
//    }
//
//    private void text(String message, Color foreground, Color background) {
//        if (SwingUtilities.isEventDispatchThread()) {
            //fixme: Discard oldest when becomes too big
//            StyledDocument document = text.getStyledDocument();
//            SimpleAttributeSet attributes = new SimpleAttributeSet();
//            if (foreground != null) {
//                StyleConstants.setForeground(attributes, foreground);
//            }
//            if (background != null) {
//                StyleConstants.setBackground(attributes, background);
//            }
//            StyleConstants.setBold(attributes, false);
//            try {
//                document.insertString(document.getLength(), message, attributes);
//            } catch (BadLocationException e) {
//                e.printStackTrace();
//            }
//            textArea.setVisible(true);
//            textArea.append(message);
//            textArea.setCaretPosition(textArea.getDocument().getLength());
//        }
    }

    private void initFontScaling() {
        this.addComponentListener(new ComponentAdapter() {
            //fixme: Do it in paint instead maybe?
            @Override
            public void componentResized(ComponentEvent e) {
                super.componentResized(e);
                scaleFontAndUpdate();
            }
        });
        scaleFontAndUpdate();
    }

    private void scaleFontAndUpdate() {
        Dimension size = getSize();
        int min = FastMath.min(size.height, size.width);
        int fontSize = min / 42;
        Font font = Pitchenga.MONOSPACED.deriveFont((float) fontSize);
        setLabelsFont(font);
        update();
    }

    public void setLabelsFont(Font font) {
        for (JComponent label : allLabelsToScale) {
            label.setFont(font);
            Dimension size = new Dimension((int) label.getMinimumSize().getWidth(), (int) label.getMinimumSize().getWidth());
            label.setPreferredSize(size);
        }
    }

    public void setTone(Pitch pitch, Color toneColor, Color pitchinessColor, float frequency) {
        this.pitch = pitch;
        this.toneColor = toneColor;
        this.pitchinessColor = pitchinessColor;
        this.frequency = frequency;
    }

    public void setTones(Fugue toneFugue, Tone... tones) {
        OpenGlCircularVisualizer.toneOverride = toneFugue == null ? null : toneFugue.pitch.tone;
        this.toneFugue = toneFugue;
        this.tones.clear();
        this.tones.addAll(Arrays.asList(tones));
        this.pitch = null;
        this.pitchinessColor = null;
    }

    public void setScaleTones(Collection<Tone> scaleTones) {
        this.scaleTones.clear();
        this.scaleTones.addAll(scaleTones);
    }

    public void setFillColor(Color fillColor) {
        this.fillColor = fillColor;
    }

    public void clear() {
        setTones(null);
        setFillColor(null);
    }

    public void update() {
        //fixme: Generify
        if (currentView == circle) {
            circle.setVisible(true);
            circle.repaint();
        } else {
            circle.setVisible(false);
        }
        if (currentView == twoOctavePiano) {
            if (Pitchenga.playButton.isSelected()) {
                //fixme: Hack
                oneOctavePiano.setVisible(true);
                twoOctavePiano.setVisible(false);
                oneOctavePiano.update();
//                circle.setVisible(true);
//                circle.repaint();
            } else {
                twoOctavePiano.setVisible(true);
                oneOctavePiano.setVisible(false);
                twoOctavePiano.update();
            }
        } else {
            oneOctavePiano.setVisible(false);
            twoOctavePiano.setVisible(false);
        }
        if (currentView == fretsFirst) {
            if (Pitchenga.playButton.isSelected() /* && !matchPitch */) {
                fretsBase.setVisible(true);
                fretsFirst.setVisible(false);
                fretsBase.update();
            } else {
                fretsFirst.setVisible(true);
                fretsBase.setVisible(false);
                fretsFirst.update();
            }
        } else {
            fretsBase.setVisible(false);
            fretsFirst.setVisible(false);
        }
        if (textPane.isVisible()) {
            textPane.repaint();
        }
    }

    @SuppressWarnings("unused")
    public final Pitch[][] FRETS_BASE = {
            {Le4, La4, Te4, Ti4, Do5,},
            {Me4, Mi4, Fa4, Fi4, So4,},
            {Te3, Ti3, Do4, Ra4, Re4,},
    };

    @SuppressWarnings("unused")
    public final Pitch[][] FRETS_FIRST = {
            {Mi5, Fa5, Fi5, So5, Le5, La5, Te5},
            {Ti4, Do5, Ra5, Re5, Me5, Mi5, Fa5},
            {So4, Le4, La4, Te4, Ti4, Do5, Ra5},
            {Re4, Me4, Mi4, Fa4, Fi4, So4, Le4},
            {La3, Te3, Ti3, Do4, Ra4, Re4, Me4},
            {Mi3, Fa3, Fi3, So3, Le3, La3, Te3},
    };

    public void flip() {
        for (int i = 0; i < views.length; i++) {
            JComponent view = views[i];
            if (currentView == view) {
                if (i == views.length - 1) {
                    currentView = views[0];
                } else {
                    currentView = views[i + 1];
                    break;
                }
            }
        }
        update();
    }

    public class Piano extends JPanel {

        private final JLabel[] labels;
        private final JPanel[] panels;
        private final JSlider[] sliders;
        private final Button[] buttons;
        private final Component frontStrut;
        private final Component rearStrut;

        public Piano(boolean twoOctaves) {
            this.setLayout(new BorderLayout());

            JPanel pianoPanel = new JPanel();
            pianoPanel.setBackground(Color.DARK_GRAY);
            pianoPanel.setBorder(BorderFactory.createLineBorder(Color.DARK_GRAY, 2));
            this.add(pianoPanel, BorderLayout.CENTER);
            pianoPanel.setLayout(new GridLayout(2, 1, 2, 2));

            JPanel blackKeysPanel = new JPanel(new BorderLayout());
            pianoPanel.add(blackKeysPanel);
            blackKeysPanel.setBackground(Color.DARK_GRAY);
            pianoPanel.add(blackKeysPanel, BorderLayout.CENTER);

            frontStrut = Box.createHorizontalStrut(55);
            blackKeysPanel.add(frontStrut, BorderLayout.EAST);
            frontStrut.setBackground(Color.DARK_GRAY);

            JPanel blackKeysPanelPanel = new JPanel();
            blackKeysPanel.add(blackKeysPanelPanel, BorderLayout.CENTER);
            blackKeysPanelPanel.setBackground(Color.DARK_GRAY);
            blackKeysPanelPanel.setLayout(new GridLayout(1, 13, 2, 2));

            rearStrut = Box.createHorizontalStrut(55);
            blackKeysPanel.add(rearStrut, BorderLayout.WEST);
            rearStrut.setBackground(Color.DARK_GRAY);

            JPanel whiteKeysPanel = new JPanel();
            pianoPanel.add(whiteKeysPanel, BorderLayout.CENTER);
            whiteKeysPanel.setBackground(Color.DARK_GRAY);
            whiteKeysPanel.setLayout(new GridLayout(1, 14, 2, 2));

            Button[] buttons = Button.values();
            List<JLabel> labelsList = new LinkedList<>();
            List<JPanel> panelsList = new LinkedList<>();
            List<JSlider> slidersList = new LinkedList<>();
            List<Button> buttonsList = new LinkedList<>();
            for (Button button : buttons) {
                if (button.row < 1 || !button.main) {
                    continue;
                }
                if (!twoOctaves && button == Button.O) {
                    break;
                }
                JPanel buttonPanel = new JPanel();
                panelsList.add(buttonPanel);
                buttonsList.add(button);
                if (button.pitch == null) {
                    blackKeysPanelPanel.add(buttonPanel);
                    buttonPanel.setBackground(Color.DARK_GRAY);
                }
                if (button.pitch != null) {
                    if (button.row == 1) {
                        blackKeysPanelPanel.add(buttonPanel);
                    } else {
                        whiteKeysPanel.add(buttonPanel);
                    }
                }
                buttonPanel.setLayout(new BoxLayout(buttonPanel, BoxLayout.Y_AXIS));
                buttonPanel.setBackground(Color.DARK_GRAY);
                if (button.pitch != null) {
                    buttonPanel.setBorder(BorderFactory.createLineBorder(button.pitch.tone.color, getBorderThickness()));
                }

                JSlider slider = createSlider();
                slidersList.add(slider);
                //fixme: When going back from full screen the label stays big vertically
                //fixme: There is a one pixel gap between the label border and the outer border in the top row
                if (button.row == 1) {
                    buttonPanel.add(Box.createVerticalGlue());
                    buttonPanel.add(slider);
                    buttonPanel.add(Box.createVerticalGlue());
                }
                JLabel toneLabel = new JLabel(button.pitch == null ? "    " : button.pitch.tone.label);
                labelsList.add(toneLabel);
                buttonPanel.add(toneLabel);
                toneLabel.setFont(Pitchenga.MONOSPACED);
                toneLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
                if (button.pitch != null) {
                    toneLabel.setForeground(button.pitch.tone.diatonic ? Color.BLACK : Color.WHITE);
                    toneLabel.setBorder(BorderFactory.createLineBorder(Color.LIGHT_GRAY, getBorderThickness() * 4));
                }
                if (button.row == 2) {
                    buttonPanel.add(Box.createVerticalGlue());
                    buttonPanel.add(slider);
                    buttonPanel.add(Box.createVerticalGlue());
                }
            }
            Display.this.allLabelsToScale.addAll(labelsList);
            this.labels = labelsList.toArray(new JLabel[0]);
            this.panels = panelsList.toArray(new JPanel[0]);
            this.sliders = slidersList.toArray(new JSlider[0]);
            this.buttons = buttonsList.toArray(new Button[0]);
        }

        private int convertPitchToSlider(Pitch pitch, float frequency) {
            //fixme: Does not need to be this hacky
            int value = 100;
            if (frequency != 0) {
                double diff = frequency - pitch.frequency;
                Pitch pitchy;
                if (diff < 0) {
                    pitchy = Pitchenga.transposePitch(pitch, 0, -1);
                } else {
                    pitchy = Pitchenga.transposePitch(pitch, 0, +1);
                }
                double pitchyDiff = FastMath.abs(pitch.frequency - pitchy.frequency);
                double accuracy = FastMath.abs(diff) / pitchyDiff;
                accuracy = accuracy * 100;
                if (pitch.frequency < frequency) {
                    value += accuracy;
                } else {
                    value -= accuracy;
                }
            }
            return value;
        }

        private int getBorderThickness() {
            int thickness = (FastMath.min(getWidth(), getHeight()) / 7) / 50;
            if (thickness == 0) {
                thickness = 1;
            }
            return thickness;
        }

        private JSlider createSlider() {
            JSlider slider = new JSlider(JSlider.VERTICAL);
            slider.setVisible(false);
            slider.setEnabled(false);
            slider.setValue(0);
            slider.getModel().setMinimum(convertPitchToSlider(Pitch.Ra0, Do0.frequency));
            slider.getModel().setMaximum(convertPitchToSlider(Pitch.Ra0, Re0.frequency));
            slider.setValue(100);
            slider.setPaintTicks(false);
            slider.setPaintLabels(true);
            Dictionary<Integer, JLabel> labels = new Hashtable<>();
            labels.put(0, new JLabel(""));
            labels.put(100, new JLabel("-"));
            labels.put(200, new JLabel(""));
            slider.setLabelTable(labels);
            return slider;
        }

        public void update() {
            Tone tone = Display.this.pitch == null ? null : Display.this.pitch.tone;
            Color toneColor = Display.this.toneColor;
            Color pitchyColor = Display.this.pitchinessColor;
            if (panels == null || panels[0] == null) {
                return;
            }

            int borderThickness = getBorderThickness();
            frontStrut.setPreferredSize(new Dimension(panels[0].getWidth() / 2, 0));
            rearStrut.setPreferredSize(new Dimension(panels[0].getWidth() / 2, 0));

            for (int i = 0; i < panels.length; i++) {
                Pitch pitch = buttons[i].pitch;
                JPanel panel = panels[i];
                JLabel label = labels[i];
                JSlider slider = sliders[i];
                Button button = buttons[i];
                if (pitch != null) {
                    Tone myTone = pitch.tone;
                    if (myTone == tone && toneColor != null && pitchyColor != null) {
                        label.setBorder(BorderFactory.createLineBorder(pitchyColor, getBorderThickness() * 4));
                        panel.setBackground(toneColor);
                        int sliderValue = convertPitchToSlider(Display.this.pitch, frequency);
                        slider.setValue(sliderValue);
                        slider.setVisible(true);
                    } else {
                        slider.setVisible(false);
                        //fixme: Make less hacky maybe?
                        boolean hideForDo = (Fugue.DoDo.equals(toneFugue) && !button.equals(Button.K))
                                || (Fugue.Do.equals(toneFugue) && !button.equals(Button.A));
//                        Pitchenga.debug("toneFugue=" + toneFugue + ", hideForLowerDo=" + hideForDo);
                        if (!hideForDo && tones.contains(myTone)) {
                            panel.setBackground(myTone.color);
                            label.setBorder(BorderFactory.createLineBorder(myTone.color, getBorderThickness() * 4));
                        } else {
                            label.setBorder(BorderFactory.createLineBorder(pitch.tone.diatonic ? Color.DARK_GRAY : Color.BLACK, getBorderThickness() * 4));
                            panel.setBackground(pitch.tone.diatonic ? Color.DARK_GRAY : Color.BLACK);
                        }
                    }
                }
            }
        }

    }

    @SuppressWarnings("unused")
    private class Frets extends JPanel {

        private final JPanel[][] panels;
        private final Pitch[][] frets;

        public Frets(Pitch[][] frets) {
            super(new GridLayout(frets.length, frets[0].length, 4, 4));
            this.frets = frets;
            setBackground(Color.BLACK);
            panels = new JPanel[frets.length][];

            for (int i = 0; i < frets.length; i++) {
                Pitch[] row = frets[i];
                panels[i] = new JPanel[row.length];
                for (int j = 0; j < row.length; j++) {
                    Pitch pitch = row[j];
                    String label = pitch != null ? pitch.tone.label : "    ";
                    Color color = pitch != null ? pitch.tone.color : Color.BLACK;

                    JPanel panel = new JPanel();
                    panels[i][j] = panel;
                    this.add(panel);
                    panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));
//                    panel.setLayout(new BorderLayout());
                    panel.setBackground(Color.BLACK);
//                        panel.setBackground(pitch != null && pitch.tone.diatonic ? Color.DARK_GRAY : Color.BLACK);
                    panel.setBorder(BorderFactory.createLineBorder(color, getBorderThickness()));

//                    JPanel labelPanel = new JPanel();
//                    labelPanel.setBackground(null);
//                    panel.add(labelPanel, BorderLayout.CENTER);
//                    labelPanel.setBorder(BorderFactory.createLineBorder(Color.GREEN));

                    panel.add(Box.createVerticalGlue());
                    JLabel toneLabel = new JLabel(label);
                    allLabelsToScale.add(toneLabel);
                    panel.add(toneLabel, BorderLayout.CENTER);
//                    labelPanel.add(toneLabel, BorderLayout.CENTER);
                    toneLabel.setFont(Pitchenga.MONOSPACED);
                    toneLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
                    toneLabel.setAlignmentY(Component.CENTER_ALIGNMENT);
//                    toneLabel.setForeground(pitch != null && pitch.tone.diatonic ? Color.BLACK : Color.WHITE);
                    toneLabel.setBackground(Color.BLACK);
//                    toneLabel.setOpaque(true);
                    toneLabel.setHorizontalTextPosition(SwingConstants.CENTER);
                    panel.setVisible(pitch != null);
                    if (pitch != null) {
                        toneLabel.setForeground(pitch.tone.diatonic ? Color.BLACK : Color.WHITE);
                    }
                    panel.add(Box.createVerticalGlue());


                    //fixme: Handle clicks
//                colorPanel.addMouseListener(new MouseAdapter() {
//                    @Override
//                    public void mousePressed(MouseEvent e) {
//                        handleButton(theButton, true);
//                    }
//                    @Override
//                    public void mouseReleased(MouseEvent e) {
//                        handleButton(theButton, false);
//                    }
//                });
                }

                int borderThickness = getBorderThickness() * 4;
                this.setBorder(BorderFactory.createLineBorder(Color.DARK_GRAY, borderThickness));
                setLayout(new GridLayout(frets.length, frets[0].length, borderThickness, borderThickness));
            }
        }

        private int getBorderThickness() {
            int thickness = (FastMath.min(getWidth(), getHeight()) / frets[0].length) / 18;
            if (thickness == 0) {
                thickness = 1;
            }
            return thickness;
        }

        public void update() {
            Tone tone = Display.this.pitch == null ? null : Display.this.pitch.tone;
            Color toneColor = Display.this.toneColor;
            Color pitchyColor = Display.this.pitchinessColor;
            Color fillColor = Display.this.fillColor;
            if (fillColor == null) {
                fillColor = Color.DARK_GRAY;
            }
            setBackground(fillColor);

            if (panels == null || panels[0] == null) {
                return;
            }

            int borderThickness = getBorderThickness();
            this.setBorder(BorderFactory.createLineBorder(fillColor, borderThickness * 4));
            setLayout(new GridLayout(frets.length, frets[0].length, borderThickness * 4, borderThickness * 4));

            for (int i = 0; i < frets.length; i++) {
                Pitch[] row = frets[i];
                for (int j = 0; j < row.length; j++) {
                    Pitch pitch = row[j];
                    JPanel panel = panels[i][j];
                    if (pitch != null) {
                        Tone myTone = pitch.tone;
                        if (myTone == tone && toneColor != null && pitchyColor != null) {
                            //fixme: Pitchiness color around the label instead
//                            panel.setBorder(BorderFactory.createLineBorder(pitchyColor, borderThickness * 2));
                            panel.setBorder(BorderFactory.createLineBorder(Color.BLACK, borderThickness * 2));
                            panel.setBackground(toneColor);
                        } else {
                            if (tones.contains(myTone)) {
                                panel.setBorder(BorderFactory.createLineBorder(Color.BLACK, borderThickness));
                                panel.setBackground(myTone.color);
                            } else {
                                panel.setBorder(BorderFactory.createLineBorder(myTone.color, borderThickness));
//                                panel.setBackground(Color.BLACK);
                                panel.setBackground(myTone.diatonic ? Color.DARK_GRAY : Color.BLACK);
                            }
                        }
                    }
                }
            }
        }
    }

    @SuppressWarnings("unused")
    private class Circle extends JPanel {

        private final List<JLabel> labels = new ArrayList<>(TONES.length);

        public Circle() {
            for (Tone tone : TONES) {
                JLabel label = new JLabel(tone.label);
                label.setFont(Pitchenga.MONOSPACED);
                label.setForeground(tone.diatonic ? Color.BLACK : Color.WHITE);
                label.setPreferredSize(new Dimension((int) label.getPreferredSize().getWidth(), (int) label.getPreferredSize().getWidth()));
                label.setOpaque(false);
                this.add(label);
                labels.add(label);
                allLabelsToScale.add(label);
            }
        }

        @Override
        protected void paintComponent(Graphics graphics) {
            Tone tone = Display.this.pitch == null ? null : Display.this.pitch.tone;
            Color toneColor = Display.this.toneColor;
            Color pitchyColor = Display.this.pitchinessColor;
            Color fillColor = Display.this.fillColor;
            if (fillColor == null) {
                fillColor = Color.DARK_GRAY;
            }

            Dimension bounds = this.getSize();
//            graphics.setColor(fillColor);
            graphics.setColor(Color.BLACK);
            graphics.fillRect(0, 0, bounds.width, bounds.height);

            int offset;
            int fullSide;
            if (bounds.height < bounds.width) {
                fullSide = bounds.height;
                offset = (bounds.width - bounds.height) / 2;
            } else {
                fullSide = bounds.width;
                offset = 0;
            }
            int side = fullSide - fullSide / 4;
            int halfSide = side / 2;
            int diameter = (int) (side / 4.7);
            int radius = diameter / 2;
            int halfRadius = radius / 2;
            int gap = (int) (halfRadius / 1.5);
            int outerDiameter = fullSide - gap * 2;

            graphics.setColor(Color.BLACK);
            graphics.fillOval(gap + offset, gap, outerDiameter, outerDiameter);

            for (int i = 0; i < TONES.length; i++) {
                Tone myTone = TONES[i];
                double phi = (i * FastMath.PI * 2) / TONES.length;
                int x;
                int y;
//                if (myTone.diatonic) {
                x = (int) FastMath.round(gap / 4.0 + halfSide * FastMath.sin(phi) + halfSide - halfRadius + radius);
                y = (int) FastMath.round(gap / 4.0 + halfSide * FastMath.cos(phi) + halfSide - halfRadius + radius);
//                } else {
//                    x = (int) FastMath.round(gap / 4.0 + halfSide * FastMath.sin(phi) + halfSide - halfRadius + radius);
//                    y = (int) FastMath.round(gap / 4.0 + halfSide * FastMath.cos(phi) + halfSide - halfRadius + radius);
//                }

                Color labelColor;
                if (myTone == tone && toneColor != null && pitchyColor != null) {
                    labelColor = myTone.diatonic ? Color.BLACK : Color.WHITE;
                    triangle(graphics, offset, gap, fullSide, halfSide, radius, halfRadius, i, toneColor, true, myTone.diatonic);
                    graphics.setColor(pitchyColor);
                    graphics.fillOval(x + offset, y, diameter, diameter);
                    graphics.setColor(toneColor);
                    graphics.fillOval(x + offset + halfRadius / 2, y + halfRadius / 2, diameter - halfRadius, diameter - halfRadius);
                } else {
                    if (tones.contains(myTone)) {
                        labelColor = myTone.diatonic ? Color.BLACK : Color.WHITE;
                        triangle(graphics, offset, gap, fullSide, halfSide, radius, halfRadius, i, myTone.color, true, myTone.diatonic);
                        graphics.setColor(myTone.color);
                        graphics.fillOval(x + offset, y, diameter, diameter);
                    } else {
//                        labelColor = Color.WHITE;
                        labelColor = myTone.diatonic ? Color.BLACK : Color.WHITE;
                        double phi2 = (i * FastMath.PI * 2) / TONES.length;
                        int x2 = (int) FastMath.round(gap / 4.0 + halfSide * FastMath.sin(phi2) + halfSide + halfRadius + radius);
                        int y2 = (int) FastMath.round(gap / 4.0 + halfSide * FastMath.cos(phi2) + halfSide + halfRadius + radius);
//                        triangle(graphics, offset, gap, fullSide, halfSide, radius, halfRadius, i, myTone.color, false, myTone.diatonic);
//                        triangle(graphics, offset, gap, fullSide, halfSide, radius, halfRadius, i, myTone.color, false, myTone.diatonic);
                        graphics.setColor(myTone.color);
                        graphics.drawLine(x2 + offset, y2, fullSide / 2 + offset, fullSide / 2);
                        graphics.fillOval(x + offset, y, diameter, diameter);
                        int thickness;
                        if (scaleTones.contains(myTone)) {
                            thickness = 1 + gap / 2;
                        } else {
                            thickness = 1 + gap / 12;
                        }
//                        graphics.setColor(myTone.diatonic ? MORE_DARK : Color.BLACK);
                        graphics.setColor(myTone.diatonic ? DARK : Color.BLACK);
//                        graphics.setColor(Color.BLACK);
                        graphics.fillOval(offset + x + thickness, y + thickness, diameter - thickness * 2, diameter - thickness * 2);
                    }
                }

                JComponent label = labels.get(i);
                label.setForeground(labelColor);
                int width = label.getWidth();
                int height = label.getHeight();
                Graphics labelGraphics = graphics.create(offset + x + radius - width / 2, y + radius - height / 2, width, height);
//                labelGraphics.setColor(myTone.diatonic ? Color.DARK_GRAY : Color.BLACK);
//                labelGraphics.fillOval(0, 0, width, height);
                label.paint(labelGraphics);
            }
            //fixme: Repainting the text pane this way causes repainting everything continuously in the loop
//            textPane.repaint();
        }

        private void triangle(Graphics graphics, int offset, int gap, int fullSide, int halfSide, int radius, int halfRadius, int i, Color color, boolean fill, boolean diatonic) {
            double phi2 = ((i - 0.2) * FastMath.PI * 2) / TONES.length;
            double phi3 = ((i + 0.2) * FastMath.PI * 2) / TONES.length;
            int x2;
            int y2;
            int x3;
            int y3;
//            if (diatonic) {
            x2 = (int) FastMath.round(gap / 4.0 + halfSide * FastMath.sin(phi2) + halfSide + halfRadius + radius);
            y2 = (int) FastMath.round(gap / 4.0 + halfSide * FastMath.cos(phi2) + halfSide + halfRadius + radius);
            x3 = (int) FastMath.round(gap / 4.0 + halfSide * FastMath.sin(phi3) + halfSide + halfRadius + radius);
            y3 = (int) FastMath.round(gap / 4.0 + halfSide * FastMath.cos(phi3) + halfSide + halfRadius + radius);
//            } else {
//                x2 = (int) FastMath.round(gap / 4.0 + halfSide * FastMath.sin(phi2) + halfSide + halfRadius + radius);
//                y2 = (int) FastMath.round(gap / 4.0 + halfSide * FastMath.cos(phi2) + halfSide + halfRadius + radius);
//                x3 = (int) FastMath.round(gap / 4.0 + halfSide * FastMath.sin(phi3) + halfSide + halfRadius + radius);
//                y3 = (int) FastMath.round(gap / 4.0 + halfSide * FastMath.cos(phi3) + halfSide + halfRadius + radius);
//            }
            int[] xPoints = {x2 + offset, x3 + offset, fullSide / 2 + offset};
            int[] yPoints = {y2, y3, fullSide / 2};
            if (fill) {
                graphics.setColor(color);
                graphics.fillPolygon(xPoints, yPoints, 3);
            } else {
//                graphics.setColor(diatonic ? Color.DARK_GRAY : Color.BLACK);
                graphics.setColor(Color.BLACK);
                graphics.fillPolygon(xPoints, yPoints, 3);
                graphics.setColor(color);
                graphics.drawPolygon(xPoints, yPoints, 3);
            }
        }

        @Override
        protected void paintBorder(Graphics g) {
        }

        @Override
        protected void paintChildren(Graphics g) {
        }

    }

}