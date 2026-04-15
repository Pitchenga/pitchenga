package com.pitchenga;

import org.apache.commons.math3.util.FastMath;

import javax.swing.*;
import java.awt.*;

public class Label extends JLabel {
    private final String text;
    private final double rotateDegrees;
    private final int shiftX;
    private final int shiftY;

    public Label(String text, double rotateDegrees, int shiftX, int shiftY, boolean outline) {
        super(text);
        this.text = text;
        this.rotateDegrees = rotateDegrees;
        this.shiftX = shiftX;
        this.shiftY = shiftY;
    }

    public void paintComponent(Graphics g) {
        Graphics2D graphics = (Graphics2D) g;
        graphics.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

        graphics.setColor(getForeground());
        graphics.rotate(FastMath.toRadians(rotateDegrees));
        graphics.drawString(text, shiftX, shiftY);

        //fixme: Need to calculate the right translate offsets
//        if (outline) {
//            FontRenderContext fontRenderContext = graphics.getFontRenderContext();
//            GlyphVector glyphVector = getFont().createGlyphVector(fontRenderContext, text);
//            graphics.setColor(Color.DARK_GRAY);
//            graphics.draw(glyphVector.getOutline());
//        }

//        int width = getSize().width;
//        int height = getSize().height;
//
//        graphics.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
//                RenderingHints.VALUE_ANTIALIAS_ON);
//
//        graphics.setRenderingHint(RenderingHints.KEY_RENDERING,
//                RenderingHints.VALUE_RENDER_QUALITY);
//
//
//
//        FontRenderContext fontRenderContext = graphics.getFontRenderContext();
//        TextLayout textLayout = new TextLayout(text, getFont(), fontRenderContext);
//        Shape outline = textLayout.getOutline(null);
//        Rectangle outlineBounds = outline.getBounds();
//        AffineTransform transform = graphics.getTransform();
//        transform.translate(width / 2.0 - (outlineBounds.width / 2.0), height / 2.0
//                + (outlineBounds.height / 2.0));
//        graphics.transform(transform);
//        graphics.setColor(Color.blue);
//        graphics.draw(outline);
//        graphics.setClip(outline);
    }
}