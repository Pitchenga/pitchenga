package com.harmoneye.viz;

import static com.jogamp.opengl.GL.GL_BACK;
import static com.jogamp.opengl.GL.GL_RGBA;
import static com.jogamp.opengl.GL.GL_TRIANGLES;
import static com.jogamp.opengl.GL.GL_UNSIGNED_BYTE;
import static com.pitchenga.Pitchenga.CHROMATIC_SCALE;
import static com.pitchenga.Pitchenga.TARSOS;
import static com.pitchenga.Pitchenga.isPlaying;
import static com.pitchenga.domain.Pitch.Do1;
import static com.pitchenga.domain.Pitch.Do7;
import static java.awt.Color.BLACK;
import static java.awt.Color.WHITE;

import com.harmoneye.analysis.AnalyzedFrame;
import com.harmoneye.analysis.ExpSmoother;
import com.harmoneye.math.cqt.CqtContext;
import com.jogamp.opengl.GL;
import com.jogamp.opengl.GL2;
import com.jogamp.opengl.GLAutoDrawable;
import com.jogamp.opengl.GLCapabilities;
import com.jogamp.opengl.GLEventListener;
import com.jogamp.opengl.GLProfile;
import com.jogamp.opengl.awt.GLJPanel;
import com.jogamp.opengl.util.Animator;
import com.jogamp.opengl.util.GLBuffers;
import com.jogamp.opengl.util.awt.TextRenderer;
import com.pitchenga.Pitchenga;
import com.pitchenga.domain.Fugue;
import com.pitchenga.domain.Pitch;
import com.pitchenga.domain.Scale;
import com.pitchenga.domain.Tone;

import org.apache.commons.lang3.tuple.Pair;
import org.apache.commons.math3.util.FastMath;

import java.awt.Color;
import java.awt.Component;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.stream.Collectors;
import java.util.zip.ZipEntry;
import java.util.zip.ZipOutputStream;

import javax.imageio.ImageIO;

// TODO: rewrite to use vertex buffers instead of immediate mode
public class OpenGlCircularVisualizer implements SwingVisualizer<AnalyzedFrame>, GLEventListener {

    public static final boolean DRAW_SNOWFLAKE = false;
    //        public static final boolean DRAW_SNOWFLAKE = true;
    public static final int SLIDER_MIN = Pitchenga.convertPitchToSlider(Do1, Do1.frequency);
    public static final int SLIDER_MAX = Pitchenga.convertPitchToSlider(Do7, Do7.frequency);
    //fixme: Un-hack
    public static volatile OpenGlCircularVisualizer INSTANCE;

    protected static final String[] HALFTONE_NAMES = Arrays.stream(Tone.values()).map(tone -> tone.name).toArray(String[]::new);
    public static final Color LESS_DARK = new Color(73, 73, 73);
    public static final Color DARK = new Color(42, 42, 42);
    private static final Color MORE_DARK = new Color(10, 10, 10);

    //recording
    public static final boolean RECORD_VIDEO = false;
    //    public static final boolean RECORD_VIDEO = true;
    public static volatile Pitch currentPitch;
    public static volatile Pitch previousPitch;
    private static volatile PrintStream recordVideoPrintStream;
    private static volatile ZipOutputStream recordVideoZipStream;
    // capturing scales
    private int scaleCounter = 0;
    @SuppressWarnings("unchecked")
    private static final Pair<String, Scale>[] SCALES = new Pair[]{
            Pair.of("00", Scale.Do3Maj),
            Pair.of("01", Scale.So3Maj),
            Pair.of("02", Scale.Re3Maj),
            Pair.of("03", Scale.La3Maj),
            Pair.of("04", Scale.Mi3Maj),
            Pair.of("05", Scale.Ti3Maj),
            Pair.of("06", Scale.Fi3Maj),
            Pair.of("07", Scale.Ra3Maj),
            Pair.of("08", Scale.Le3Maj),
            Pair.of("09", Scale.Me3Maj),
            Pair.of("10", Scale.Te3Maj),
            Pair.of("11", Scale.Fa3Maj)};
    private static final AtomicBoolean printScreen = new AtomicBoolean(false);

    //Video playback
    public static final AtomicBoolean videoNewFugue = new AtomicBoolean(true);
    private static volatile Fugue videoCurrentFugue;
    private static volatile Fugue videoPreviousFugue;
    private static final AtomicInteger videoFrameNumber = new AtomicInteger(-1);
    private static volatile ExpSmoother videoSmoother = new ExpSmoother(CqtContext.binsPerHalftone, 0.2);

    public static volatile Tone toneOverrideTarsos;
    public static int sliderOverrideTarsos;
    public static Color guessColorOverrideTarsos;
    public static Color pitchinessColorOverrideTarsos;
    //    public static volatile Set<String> scale = getToneNames(SCALES[0].right);
    private static volatile Set<String> scale = Collections.emptySet();
    private static volatile String scaleName = "";


    private int pitchStep = 1;

    private double[] binVelocities;

    private final ColorFunction colorFunction = new ColorFunction();
    private final Component component;
    private int binsPerHalftone;
    private double stepAngle;

    private TextRenderer renderer;
    public static volatile Tone toneOverride;
    public static volatile String text;

    public OpenGlCircularVisualizer() {
        INSTANCE = this;
        GLProfile glp = GLProfile.getDefault();
        GLCapabilities caps = new GLCapabilities(glp);
        caps.setSampleBuffers(true);
        GLJPanel canvas = new GLJPanel(caps);
        canvas.addGLEventListener(this);
        component = canvas;
        Animator animator = new Animator();
        animator.add(canvas);
        animator.start();
        // TODO: stop the animator if the computation is stopped

        if (DRAW_SNOWFLAKE) {
            canvas.addMouseListener(new MouseAdapter() {
                @Override
                public void mouseClicked(MouseEvent e) {
                    printScreen.set(true);
                }
            });
        }
    }

    @Override
    public void update(AnalyzedFrame pcProfile) {
        if (pcProfile == null) {
            return;
        }
        CqtContext cqtContext = pcProfile.getCqtContext();
        binsPerHalftone = cqtContext.getBinsPerHalftone();

        double[] octaveBins = pcProfile.getOctaveBins();
        if (octaveBins == null || octaveBins.length == 0) {
            return;
        }

        if (DRAW_SNOWFLAKE) {
            drawSnowflake();
        } else {
            if (binVelocities == null || binVelocities.length != octaveBins.length) {
                videoSmoother = new ExpSmoother(octaveBins.length, 0.1);
            }
            binVelocities = octaveBins;
        }
        double segmentCountInv = 1.0 / binVelocities.length;
        stepAngle = 2 * FastMath.PI * segmentCountInv;

        updateStars();
    }

    private void drawSnowflake() {
        double[] velocities = new double[108];
        for (int i = 0; i < velocities.length; i++) {
            Tone myTone = getTone(i, -1.0);
            double velocity = 0.0;
            if (scale.isEmpty() || (myTone != null && scale.contains(myTone.name))) {
                int ii = 5 + i;
                int mod = ii % 9;
                if (mod == 0) {
                    velocity = 1.2;
                } else if (mod == 1 || mod == 8) {
                    velocity = 0.87;
                } else if (mod == 2 || mod == 7) {
                    velocity = 0.72;
                } else if (mod == 3 || mod == 6) {
                    velocity = 0.61;
                } else if (scale.isEmpty()) {
                    velocity = 0.46;
                }
            } else {
                velocity = 0.0;
            }
            velocities[i] = velocity;
        }
        binVelocities = velocities;
    }

    private void fadeOut() {
        if (binVelocities == null) {
            return;
        }
        for (int i = 0; i < binVelocities.length; i++) {
            binVelocities[i] = binVelocities[i] * 0.8;
            binVelocities = videoSmoother.smooth(binVelocities);
        }
    }

    @Override
    public void setPitchStep(int i) {
        this.pitchStep = i;
    }

    @Override
    public void display(GLAutoDrawable drawable) {
        GL2 gl = drawable.getGL().getGL2();
        gl.glClearColor(0f, 0f, 0f, 1f);
        gl.glClear(GL.GL_COLOR_BUFFER_BIT);
        playVideo();
        if (Pitchenga.isPlaying()) { //fixme: Move updateStars from update to here completely
            updateStars();
        }

        List<Pair<Integer, Double>> indexToVelocityPairs = new LinkedList<>();
        int[] binOrders = null;
        if (binVelocities != null) {
            for (int i = 0; i < binVelocities.length; i++) {
                double velocity = binVelocities[i];
                indexToVelocityPairs.add(Pair.of(i, velocity));
            }
            indexToVelocityPairs.sort(Comparator.comparingDouble(Pair::getRight));
            indexToVelocityPairs = new ArrayList<>(indexToVelocityPairs);
            binOrders = new int[binVelocities.length];
            for (int i = 0; i < binVelocities.length; i++) {
                Pair<Integer, Double> indexToVelocityPair = indexToVelocityPairs.get(i);
                binOrders[indexToVelocityPair.getLeft()] = i;
            }
        }

        int biggestBinNumber = binOrders == null ? -1 : indexToVelocityPairs.get(indexToVelocityPairs.size() - 1).getLeft();
        Tone tone = getTone(biggestBinNumber, 0.4);
//        debug("biggestBinNumber=" + biggestBinNumber + ", tone=" + tone);

        drawFrame(gl);
        gl.glBegin(GL_TRIANGLES);
        drawBins(gl, binOrders, biggestBinNumber);
        if (tone != null) {
            drawTuner(gl);
        }
        gl.glEnd();
        drawLabels(drawable, tone);

        printScreen(gl, 1080, 1080);
        recordVideo();
    }

    private void playVideo() {
        if (RECORD_VIDEO) {
            return;
        }
        if (!Pitchenga.isPlaying()) {
            return;
        }
        if (Pitchenga.showSeriesHint) {
            Fugue previous = videoPreviousFugue;
            Fugue current = videoCurrentFugue;
            if (current != null) {
                if (videoNewFugue.compareAndSet(true, false)) {
                    videoPreviousFugue = current;
                    videoFrameNumber.set(16);
                }
                int frameNumber = videoFrameNumber.incrementAndGet();
                double[][] video = current.pitch.video;
                if (video != null) {
                    if (frameNumber >= video.length) {
                        fadeOut();
                    } else {
                        binVelocities = video[frameNumber];
                        binVelocities = videoSmoother.smooth(binVelocities);
                    }
                } else {
                    fadeOut();
                }
            } else {
                fadeOut();
            }
//            Pitchenga.debug("current=" + current + ", previous=" + previous + ", bins=" + Arrays.toString(binVelocities));
        } else {
            fadeOut();
        }
    }

    public static void setVideoCurrentFugue(Fugue newFugue) {
        videoNewFugue.set(true);
        videoCurrentFugue = newFugue;
    }

    @SuppressWarnings("unused")
    private void recordVideo() {
        if (!RECORD_VIDEO) {
            return;
        }
        Pitch previous = previousPitch;
        Pitch current = currentPitch;
        try {
            if (current != null) {
                if (previous != current) {
                    previousPitch = current;
                    if (recordVideoPrintStream != null) {
                        recordVideoPrintStream.flush();
                        recordVideoZipStream.closeEntry();
                        recordVideoPrintStream.close();
                    }
                    File logDir = new File(System.getProperty("user.home") + "/dev/pitchenga/src/main/resources/video/");
                    //noinspection ResultOfMethodCallIgnored
                    logDir.mkdirs();
                    //fixme: rename the wav files the same way
                    File logFile = new File(logDir, current.number + ".zip");
                    if (logFile.exists()) {
                        //noinspection ResultOfMethodCallIgnored
                        logFile.delete();
                    }
                    boolean newFile = logFile.createNewFile();
                    if (!newFile) {
                        throw new RuntimeException("Failed creating log file=" + logFile.getCanonicalPath());
                    }
                    recordVideoZipStream = new ZipOutputStream(new FileOutputStream(logFile));
                    recordVideoZipStream.putNextEntry(new ZipEntry(current.number + ".txt"));
                    recordVideoPrintStream = new PrintStream(recordVideoZipStream);
                }
                if (binVelocities != null && binVelocities.length > 0) {
                    for (double binVelocity : binVelocities) {
                        recordVideoPrintStream.print(binVelocity);
                        recordVideoPrintStream.print(" ");
                    }
                    recordVideoPrintStream.println();
                }
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    private Tone getTone(int binNumber, double threshold) {
        if (binVelocities == null || binVelocities.length == 0 || binNumber < 0 || binNumber >= binVelocities.length) {
            return null;
        }
        double binVelocity = binVelocities[binNumber];
        Tone tone = null;
        if (binVelocity > threshold) {
            double toneRatio = (double) binNumber / ((double) binVelocities.length / (double) Tone.values().length);
            int toneNumber = (int) toneRatio;
            if (toneNumber >= 0 && toneNumber <= Tone.values().length + 1) {
                tone = Tone.values()[toneNumber];
            }
        }
        return tone;
    }

    private void drawTuner(GL2 gl) {
        if (!TARSOS || DRAW_SNOWFLAKE) {
            return;
        }
        if (binsPerHalftone == 0) {
            return;
        }
        if (Pitchenga.isPlaying() /* && !Pitchenga.showSeriesHint*/) {
            return;
        }

        int slider = sliderOverrideTarsos;
        slider = slider * 2;
        slider = slider % SLIDER_MAX;
        double halfToneCountInv = 1.0 / SLIDER_MIN;
        double angle = 2 * FastMath.PI * (slider * halfToneCountInv);

        Color color = guessColorOverrideTarsos;
        if (color == null) {
            color = BLACK;
        }

        double startAngle = angle - 0.4 * stepAngle;
        double sinStartAngle = FastMath.sin(startAngle);
        double cosStartAngle = FastMath.cos(startAngle);

        double endAngle = angle + 0.4 * stepAngle;
        double sinEndAngle = FastMath.sin(endAngle);
        double cosEndAngle = FastMath.cos(endAngle);

        double outerRadius = 0.98;
        double outerOuterRadius = outerRadius + 0.02;
        gl.glBegin(GL.GL_TRIANGLES);
        gl.glColor3ub((byte) color.getRed(),
                (byte) color.getGreen(),
                (byte) color.getBlue());

        double innerRadius;
        innerRadius = outerRadius;
        outerRadius = outerRadius - 0.04;

        double centerAngle = angle - 0.000000001 * stepAngle;
        double sinCenterAngle = FastMath.sin(centerAngle);
        double cosCenterAngle = FastMath.cos(centerAngle);
        gl.glVertex2d(outerRadius * sinCenterAngle, outerRadius * cosCenterAngle);
        gl.glVertex2d(innerRadius * sinStartAngle, innerRadius * cosStartAngle);
        gl.glVertex2d(innerRadius * sinEndAngle, innerRadius * cosEndAngle);
        //fixme: draw a rectangle instead of two triangles
        gl.glVertex2d(outerOuterRadius * sinCenterAngle, outerOuterRadius * cosCenterAngle);
        gl.glVertex2d(innerRadius * sinStartAngle, innerRadius * cosStartAngle);
        gl.glVertex2d(innerRadius * sinEndAngle, innerRadius * cosEndAngle);
        gl.glEnd();
    }

    private void drawFrame(GL2 gl) {
        Color color;
        double halfToneCountInv = 1.0 / HALFTONE_NAMES.length;
        gl.glLineWidth(2f);
        gl.glBegin(GL.GL_LINES);
        for (int i = 0; i < HALFTONE_NAMES.length; i++) {
            Tone tone = Tone.values()[i];
            color = tone.color;
//            if (Pitchenga.isPlaying() && !scale.isEmpty() && !scale.contains(tone.name)) {
            if (!scale.isEmpty() && !scale.contains(tone.name)) {
                color = MORE_DARK;
            }
            gl.glColor3ub((byte) color.getRed(),
                    (byte) color.getGreen(),
                    (byte) color.getBlue());

            double angle = 2 * FastMath.PI * (i * halfToneCountInv);
            double radius = 0.70;
            double sin = FastMath.sin(angle);
            double cos = FastMath.cos(angle);
            double x = radius * sin;
            double y = radius * cos;
            gl.glVertex2d(x, y);
            gl.glVertex2d(0, 0);

            radius = 1;
            x = radius * sin;
            y = radius * cos;
            gl.glVertex2d(x, y);
            radius = 0.935;
            x = radius * sin;
            y = radius * cos;
            gl.glVertex2d(x, y);
        }
        gl.glEnd();
    }

    private void drawBins(GL2 gl, int[] binOrders, int biggestBinNumber) {
        if (binVelocities == null || binVelocities.length == 0) {
            return;
        }

        double radius = 0.8;
        double angle = 0.5 * (1 - binsPerHalftone) * stepAngle;
        for (int i = 0; i < binVelocities.length; i++, angle += stepAngle) {
            //fixme: Why was this here?
//            int pitchClass = i / binsPerHalftone;
//            int binInPitchClass = i % binsPerHalftone;
//            int movedPitchClass = (pitchClass * pitchStep) % halftoneCount;
//            int index = movedPitchClass * binsPerHalftone + binInPitchClass;
//            double velocity = binVelocities[index];
            double velocity = binVelocities[i];

            int ii = i - 4;
            double toneRatio = ii / ((double) binVelocities.length / (double) Tone.values().length);

            Color color = colorFunction.toColor(velocity, toneRatio);
            if (!isPlaying() && binOrders != null) {
                int binOrder = binOrders[i];
                velocity *= binOrder * 0.013;
                if (i == biggestBinNumber) {
                    velocity *= 1.3;
                }
                if (velocity > 1.15) {
                    velocity = 1.15;
                }
            }

            double startRadius = radius * velocity;
            double startAngle = angle - 0.5 * stepAngle;
            double sinStartAngle = FastMath.sin(startAngle);
            double cosStartAngle = FastMath.cos(startAngle);

            double endRadius = radius * velocity;
            double endAngle = angle + 0.5 * stepAngle;
            double sinEndAngle = FastMath.sin(endAngle);
            double cosEndAngle = FastMath.cos(endAngle);

            gl.glColor3ub((byte) color.getRed(),
                    (byte) color.getGreen(),
                    (byte) color.getBlue());
            gl.glVertex2d(0, 0);
            gl.glVertex2d(startRadius * sinStartAngle, startRadius * cosStartAngle);
            gl.glVertex2d(endRadius * sinEndAngle, endRadius * cosEndAngle);

            drawStars(gl, angle, i, color, sinStartAngle, cosStartAngle, sinEndAngle, cosEndAngle);
            drawOuterCircleDot(gl, angle, i, color, sinStartAngle, cosStartAngle, sinEndAngle, cosEndAngle);
        }
    }

    private void drawOuterCircleDot(GL2 gl, double angle, int index, Color color, double sinStartAngle, double cosStartAngle, double sinEndAngle, double cosEndAngle) {
        gl.glColor3ub((byte) color.getRed(),
                (byte) color.getGreen(),
                (byte) color.getBlue());

        double outerRadius = 0.98;
        double innerRadius = outerRadius * 0.99 - binVelocities[index] * 0.01;
        double centerAngle = angle - 0.000000001 * stepAngle;
        double sinCenterAngle = FastMath.sin(centerAngle);
        double cosCenterAngle = FastMath.cos(centerAngle);
        gl.glVertex2d(innerRadius * sinCenterAngle, innerRadius * cosCenterAngle);
        gl.glVertex2d(outerRadius * sinStartAngle, outerRadius * cosStartAngle);
        gl.glVertex2d(outerRadius * sinEndAngle, outerRadius * cosEndAngle);
    }

    public static final int starsDepth = 64;
    //    public static final int starsDepth = 128;
    //    private static final double[][] stars = new double[128][];
    private static final double[][] stars = new double[starsDepth][];
    private static volatile int starsIndex = 0;
//    private static int starsSkipFrameCounter = 0;

    private void drawStars(GL2 gl, double angle, int binIndex, Color color, double sinStartAngle, double cosStartAngle, double sinEndAngle, double cosEndAngle) {
        int currentIndex = starsIndex(starsIndex, 0);
        for (int offset = 0; offset < stars.length; offset++) {
            int myIndex = starsIndex(currentIndex, -offset);
            double[] myStars = stars[myIndex];
            if (myStars == null) {
                continue;
            }
            double myVelocity = myStars[binIndex];
            if (myVelocity < 0.5) {
                continue;
            }
            gl.glColor3ub((byte) color.getRed(),
                    (byte) color.getGreen(),
                    (byte) color.getBlue());

            double outerRadius = 0.4 + (offset * 0.02315);
//            double outerRadius = 0.4 + ((((currentIndex   % starsDepth)+ offset) ) * 0.02);
//                    double innerRadius = outerRadius * 0.99 - binVelocities[index] * 0.01;
//            double innerRadius = outerRadius * 0.99 - binVelocities[index] * (1.0 / offset) * 0.1;
//            double innerRadius = outerRadius * 0.99 - (1.0 / (offset * 0.5)) * binVelocities[index] * 0.3;
            double innerRadius = outerRadius - FastMath.pow(myVelocity, 4) * 0.03;
//            double innerRadius = outerRadius * myVelocity * 0.1;
            if (outerRadius - innerRadius < 0.001) {
                continue;
            }
            double centerAngle = angle - 0.000000001 * stepAngle;
            double sinCenterAngle = FastMath.sin(centerAngle);
            double cosCenterAngle = FastMath.cos(centerAngle);
            gl.glVertex2d(innerRadius * sinCenterAngle, innerRadius * cosCenterAngle);
            gl.glVertex2d(outerRadius * sinStartAngle, outerRadius * cosStartAngle);
            gl.glVertex2d(outerRadius * sinEndAngle, outerRadius * cosEndAngle);


//            double startRadius = outerRadius * myVelocity;
//            double startAngle = angle - 0.5 * stepAngle;
//            double sinStartAngle = FastMath.sin(startAngle);
//            double cosStartAngle = FastMath.cos(startAngle);
//
//            double endRadius = radius * myVelocity;
//            double endAngle = angle + 0.5 * stepAngle;
//            double sinEndAngle = FastMath.sin(endAngle);
//            double cosEndAngle = FastMath.cos(endAngle);
//
//            gl.glBegin(GL.GL_TRIANGLES);
//            gl.glColor3ub((byte) color.getRed(),
//                    (byte) color.getGreen(),
//                    (byte) color.getBlue());
//            gl.glVertex2d(0, 0);
//            gl.glVertex2d(startRadius * sinStartAngle, startRadius * cosStartAngle);
//            gl.glVertex2d(endRadius * sinEndAngle, endRadius * cosEndAngle);
//            gl.glEnd();

        }
    }

    private void updateStars() {
//        if (starsSkipFrameCounter++ >= 1) {
//            starsSkipFrameCounter = 0;
//        } else {
//            return;
//        }

        int prevIndex = starsIndex;
//        double[] prevStars = stars[prevIndex];
        int newIndex = starsIndex(prevIndex, 1);
        double[] newStars = binVelocities;
//        if (prevStars == newStars) {
//            return prevIndex;
//        }
        if (stars[newIndex] == null || stars[newIndex].length != newStars.length) {
            stars[newIndex] = new double[newStars.length];
        }
        System.arraycopy(newStars, 0, stars[newIndex], 0, newStars.length);
        starsIndex = newIndex;
    }

    private void drawLabels(GLAutoDrawable drawable, Tone tone) {
        if (binVelocities == null || binVelocities.length == 0) {
            return;
        }

        int width = drawable.getSurfaceWidth();
        int height = drawable.getSurfaceHeight();

        double centerX = width / 2.0;
        double centerY = height / 2.0;
        double size = 0.97 * FastMath.min(width, height);
        double angleStep = 2 * FastMath.PI / HALFTONE_NAMES.length;
        double angle = 0;
        float scaleFactor = (float) (0.0015f * size);

        if (Pitchenga.isPlaying()) {
            tone = toneOverride;
        }
        renderer.beginRendering(width, height);
        for (int i = 0; i < HALFTONE_NAMES.length; i++, angle += angleStep) {
            int index = (i * pitchStep) % HALFTONE_NAMES.length;
            String halftoneName = HALFTONE_NAMES[index];
//            if ((Pitchenga.isPlaying() || DRAW_SNOWFLAKE) && !scale.isEmpty() && !scale.contains(halftoneName)) {
            if (!scale.isEmpty() && !scale.contains(halftoneName)) {
                continue;
            }
            Rectangle2D bounds = renderer.getBounds(halftoneName);
            int offsetX = (int) (scaleFactor * 0.5f * bounds.getWidth());
            int offsetY = (int) (scaleFactor * 0.5f * bounds.getHeight());
//            double radius = 0.43;
            double radius = 0.42;
            int x = (int) (centerX + radius * size * FastMath.sin(angle) - offsetX);
            int y = (int) (centerY + radius * size * FastMath.cos(angle) - offsetY);
            Color color;
            if (DRAW_SNOWFLAKE || (tone != null && tone.name().equalsIgnoreCase(HALFTONE_NAMES[i]))) {
                //fixme: There must be an easier way to render outlined font
                int offset;
                offset = 4;
                renderer.setColor(BLACK);
                renderer.draw3D(halftoneName, x + offset, y - offset, 0, scaleFactor);
                renderer.draw3D(halftoneName, x - offset, y + offset, 0, scaleFactor);
                renderer.draw3D(halftoneName, x + offset, y + offset, 0, scaleFactor);
                renderer.draw3D(halftoneName, x - offset, y - offset, 0, scaleFactor);

                offset = 1;
                renderer.setColor(WHITE);
                renderer.draw3D(halftoneName, x + offset, y - offset, 0, scaleFactor);
                renderer.draw3D(halftoneName, x - offset, y + offset, 0, scaleFactor);
                renderer.draw3D(halftoneName, x + offset, y + offset, 0, scaleFactor);
                renderer.draw3D(halftoneName, x - offset, y - offset, 0, scaleFactor);
                color = colorFunction.toColor(100, i);
            } else {
                Tone myTone = Pitchenga.TONE_BY_LOWERCASE_NAME.get(halftoneName);
                if (myTone != null && myTone.name.equalsIgnoreCase(scaleName)) {
                    color = LESS_DARK;
                } else {
                    color = DARK;
                }
            }
            renderer.setColor(color);
            renderer.draw3D(halftoneName, x, y, 0, scaleFactor);
        }
        if (text != null) {
            renderer.setColor(LESS_DARK);
            renderer.draw3D(text, 0, 0, 0, scaleFactor);
        }
        renderer.endRendering();
    }

    private int starsIndex(int counter, int offset) {
        int i = counter + offset;
        double[][] stars = OpenGlCircularVisualizer.stars;
        while (i >= stars.length) {
            i = i - stars.length;
        }
        while (i < 0) {
            i = i + stars.length;
        }
        return i;
    }

    @SuppressWarnings("SameParameterValue")
    private void printScreen(GL2 gl4, int width, int height) {
        if (!printScreen.getAndSet(false)) {
            return;
        }
        scaleCounter++;
        Pair<String, Scale> scalePair = SCALES[scaleCounter - 1];

        try {
            BufferedImage screenshot = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
            Graphics graphics = screenshot.getGraphics();

            ByteBuffer buffer = GLBuffers.newDirectByteBuffer(width * height * 4);

            gl4.glReadBuffer(GL_BACK);
            gl4.glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

            for (int h = 0; h < height; h++) {
                for (int w = 0; w < width; w++) {
                    graphics.setColor(new Color((buffer.get() & 0xff), (buffer.get() & 0xff),
                            (buffer.get() & 0xff)));
                    buffer.get();
                    graphics.drawRect(w, height - h, 1, 1);
                }
            }
//            BufferUtils.destroyDirectBuffer(buffer);

            String name = scalePair.getLeft() + "-" + scalePair.getRight().getScale()[0].tone.name;
            System.out.print("\"" + name + "\", ");
            File outputfile = new File("src/main/resources/scales/" + name + ".png");
            ImageIO.write(screenshot, "png", outputfile);
        } catch (IOException e) {
            e.printStackTrace();
        }
        OpenGlCircularVisualizer.scale = getToneNames(SCALES[scaleCounter].getRight());

    }

    public static void setScale(String scaleName) {
        Pitch[] pitches = CHROMATIC_SCALE;
        try {
            if (scaleName != null && !scaleName.isEmpty()) {
                Scale scale = Scale.valueOf(scaleName + "3Maj");
                pitches = scale.getScale();
            }
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        }
        OpenGlCircularVisualizer.scale = Arrays.stream(pitches).map(pitch -> pitch.tone.name).collect(Collectors.toSet());
        OpenGlCircularVisualizer.scaleName = scaleName;
    }

    private void setConstantAspectRatio(GLAutoDrawable drawable) {
        GL2 gl = drawable.getGL().getGL2();

        gl.glMatrixMode(GL2.GL_PROJECTION);
        gl.glLoadIdentity();
        double w = drawable.getSurfaceWidth();
        double h = drawable.getSurfaceHeight();
        if (w > h) {
            double aspectRatio = w / h;
            gl.glOrtho(-aspectRatio, aspectRatio, -1, 1, -1, 1);
        } else {
            double aspectRatio = h / w;
            gl.glOrtho(-1, 1, -aspectRatio, aspectRatio, -1, 1);
        }

        gl.glMatrixMode(GL2.GL_MODELVIEW);
        gl.glLoadIdentity();
    }

    @Override
    public void dispose(GLAutoDrawable drawable) {
        // TODO Auto-generated method stub
    }

    @Override
    public void init(GLAutoDrawable drawable) {
        // Synchronize the FPS with the refresh rate of the display (v-sync).
        // Otherwise, we can use the FPSAnimator instead of the plain Animator.
        GL gl = drawable.getGL();
        gl.setSwapInterval(1);

        gl.glEnable(GL.GL_LINE_SMOOTH);
        gl.glEnable(GL.GL_BLEND);
        gl.glBlendFunc(GL.GL_SRC_ALPHA, GL.GL_ONE_MINUS_SRC_ALPHA);
        gl.glHint(GL.GL_LINE_SMOOTH_HINT, GL.GL_DONT_CARE);
        gl.glLineWidth(0.5f);

        gl.glClearColor(0.25f, 0.25f, 0.25f, 1f);

        renderer = new TextRenderer(new Font("SansSerif", Font.BOLD, 50), true,
                true, null, true);
    }

    @Override
    public void reshape(GLAutoDrawable drawable, int x, int y, int width, int height) {
        setConstantAspectRatio(drawable);
    }

    @Override
    public Component getComponent() {
        return component;
    }

    private static Set<String> getToneNames(Scale scale) {
        return Arrays.stream(scale.getScale()).map(p -> p.tone.name).collect(Collectors.toSet());
    }

}