package com.harmoneye.viz;

import com.harmoneye.analysis.AnalyzedFrame;
import com.harmoneye.math.cqt.CqtContext;
import com.jogamp.opengl.*;
import com.jogamp.opengl.awt.GLJPanel;
import com.jogamp.opengl.util.Animator;
import com.pitchenga.domain.Tone;

import java.awt.*;

// TODO: rewrite to use vertex buffers instead of immediate mode

public class OpenGlLinearVisualizer implements SwingVisualizer<AnalyzedFrame>,
        GLEventListener {

//    protected static final String[] HALFTONE_NAMES = Arrays.stream(Tone.values()).map(Enum::name).toArray(String[]::new);

    private final ColorFunction colorFunction = new ColorFunction();
    private final Component component;

    private AnalyzedFrame frame;

    private boolean isLandscape;
    private double aspectRatio = 1.0;
    private final Animator animator;

    public OpenGlLinearVisualizer() {
        GLProfile glp = GLProfile.getDefault();
        GLCapabilities caps = new GLCapabilities(glp);
        caps.setSampleBuffers(true);
        GLJPanel canvas = new GLJPanel(caps);
        canvas.addGLEventListener(this);
        component = canvas;
        animator = new Animator(canvas);
        animator.start();
        // TODO: stop the animator if the computation is stopped
    }

    @Override
    public void update(AnalyzedFrame frame) {
//        if (Pitchenga.isPlaying()) {
//            return;
//        }
        this.frame = frame;
    }

    @Override
    public void setPitchStep(int i) {
    }

    @Override
    public void display(GLAutoDrawable drawable) {
//        if (Pitchenga.isPlaying()) {
//            return;
//        }
        if (frame == null) {
            return;
        }
        GL2 gl = drawable.getGL().getGL2();
        gl.glClearColor(0f, 0f, 0f, 1f);
        gl.glClear(GL.GL_COLOR_BUFFER_BIT);

        gl.glPushMatrix();
        if (isLandscape) {
            gl.glTranslated(-aspectRatio, -1, 0);
            gl.glScaled(2 * aspectRatio, 2, 0);
        } else {
            gl.glTranslated(-1, -aspectRatio, 0);
            gl.glScaled(2, 2 * aspectRatio, 0);
        }

        drawBorders(gl, frame);
//        drawBins(gl, frame.getAmplitudeSpectrumDbBins());
//        drawBins(gl, frame.getDetectedPitchClasses());
        drawBins(gl, frame.getOctaveBins());
        gl.glPopMatrix();
    }

    private void drawBorders(GL2 gl, AnalyzedFrame frame) {
        CqtContext ctx = frame.getCqtContext();

        Color color = Color.red;
        gl.glColor3ub((byte) color.getRed(),
                (byte) color.getGreen(),
                (byte) color.getBlue());
        gl.glLineWidth(0.5f);
        int halftonesPerOctave = ctx.getHalftonesPerOctave();
        // lines between bins
        gl.glBegin(GL.GL_LINES);
        int octaves = ctx.getOctaves();
        int totalBins = octaves * halftonesPerOctave;
        double xStep = 1.0 / totalBins;
        for (int i = 0; i <= totalBins; i++) {
            if (i % halftonesPerOctave != 0) {
                double x = i * xStep;
//                gl.glVertex2d(x, -1);
//                gl.glVertex2d(x, 1);
            }
        }
        gl.glEnd();

        color = Color.gray;
        gl.glColor3ub((byte) color.getRed(),
                (byte) color.getGreen(),
                (byte) color.getBlue());
        gl.glLineWidth(1f);
        gl.glBegin(GL.GL_LINES);
        totalBins = octaves;
        xStep = 1.0 / totalBins;
        for (int i = 0; i <= totalBins; i++) {
            double x = i * xStep;
            gl.glVertex2d(x, -1);
            gl.glVertex2d(x, 1);
        }
        gl.glEnd();
    }

    private void drawBins(GL2 gl, double[] binVelocities) {
        if (binVelocities == null) {
            return;
        }

        gl.glPushMatrix();

        int totalBins = binVelocities.length;
        double xStep = 1.0 / totalBins;
        //gl.glBegin(GL.GL_TRIANGLES);
        gl.glBegin(GL.GL_LINE_STRIP);
        gl.glVertex2d(0, binVelocities[0]);
        for (int i = 1; i < totalBins; i++) {
            double binVelocity = binVelocities[i];

//            double toneRatio = i / ((double) binVelocities.length / (double) Tone.values().length);
            double toneRatio = i / ((double) binVelocities.length / (double) Tone.values().length);

//            Color color = colorFunction.toColor(1.0, toneRatio);
            //fixme: Always 1 for the biggest bin
            Color color = colorFunction.toColor(binVelocity, toneRatio);
//            Color color = colorFunction.toColor(binVelocity * 0.1, toneRatio);
            gl.glColor3ub((byte) color.getRed(),
                    (byte) color.getGreen(),
                    (byte) color.getBlue());

            double xFrom = i * xStep;
            double xTo = xFrom + xStep;

//			gl.glVertex2d(xFrom, 0);
//			gl.glVertex2d(0.5 * (xFrom + xTo), yTo);
//			gl.glVertex2d(xTo, 0);

            gl.glVertex2d(xTo, binVelocity);
        }
        gl.glEnd();

        gl.glPopMatrix();

    }

    private void setConstantAspectRatio(GLAutoDrawable drawable) {
        GL2 gl = drawable.getGL().getGL2();

        gl.glMatrixMode(GL2.GL_PROJECTION);
        gl.glLoadIdentity();
        double w = drawable.getSurfaceWidth();
        double h = drawable.getSurfaceHeight();
        isLandscape = w > h;
        if (isLandscape) {
            aspectRatio = w / h;
            gl.glOrtho(-aspectRatio, aspectRatio, -1, 1, -1, 1);
        } else {
            aspectRatio = h / w;
            gl.glOrtho(-1, 1, -aspectRatio, aspectRatio, -1, 1);
        }

        gl.glMatrixMode(GL2.GL_MODELVIEW);
        gl.glLoadIdentity();
    }

    @Override
    public void dispose(GLAutoDrawable drawable) {
        if (animator != null && animator.isStarted()) {
            animator.stop();
        }
    }

    @Override
    public void init(GLAutoDrawable drawable) {
        // Synchronize the FPS with the refresh rate of the display (v-sync).
        // Otherwise we can use the FPSAnimator instead of the plain Animator.
        GL gl = drawable.getGL();
        gl.setSwapInterval(1);

        gl.glEnable(GL.GL_LINE_SMOOTH);
        gl.glEnable(GL.GL_BLEND);
        gl.glBlendFunc(GL.GL_SRC_ALPHA, GL.GL_ONE_MINUS_SRC_ALPHA);
        gl.glHint(GL.GL_LINE_SMOOTH_HINT, GL.GL_DONT_CARE);
        gl.glLineWidth(0.5f);

        gl.glClearColor(0.25f, 0.25f, 0.25f, 1f);
    }

    @Override
    public void reshape(GLAutoDrawable drawable, int x, int y, int width,
                        int height) {
        setConstantAspectRatio(drawable);
    }

    @Override
    public Component getComponent() {
        return component;
    }
}