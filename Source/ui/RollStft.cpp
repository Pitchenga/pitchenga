#include "RollStft.h"
#include "../Tone.h"
#include "../math/Math.h"
#include "../Common.h"
#include <algorithm>
#include <cmath>

RollStft::RollStft(PitchengaAudioProcessor& proc) : processor(proc) {}

void RollStft::mouseMove(const juce::MouseEvent& event) {
    mousePosition = event.getPosition();
    repaint();
}

void RollStft::mouseEnter(const juce::MouseEvent& event) {
    mousePosition = event.getPosition();
    repaint();
}

void RollStft::mouseExit(const juce::MouseEvent&) {
    mousePosition = {-1, -1};
    repaint();
}

void RollStft::updateResults(const std::vector<SpectralPeak>& peaks) {
    if (!processor.settings.isUseRollStft
        || processor.settings.isFreezeRoll
        || !isVisible()
    ) {
        return;
    }

    activePeaks = peaks;
    pumpSmoke();

    repaint();
}

void RollStft::resized() {
    const bool isHorizontal = processor.settings.isFlipRollHorizontal;
    const int logicalWidth = isHorizontal ? getHeight() : getWidth();
    const int logicalHeight = isHorizontal ? getWidth() : getHeight();
    const int physicalHeight = getHeight();

    cachedHorizontalTransform = juce::AffineTransform(0.0f, 1.0f, 0.0f, -1.0f, 0.0f, static_cast<float>(physicalHeight));
    cachedHorizontalTransformInverted = cachedHorizontalTransform.inverted();

    if (logicalWidth > 0 && logicalHeight > 0) {
        const int plotHeight = std::max(1, logicalHeight - static_cast<int>(getLabelAreaHeight()));
        // Initialize the rolling buffer whenever the window resizes
        smokeImage = juce::Image(juce::Image::ARGB, logicalWidth, plotHeight, true);
        smokeScrollOffset = 0;
        buildFrame();
    }
}

float RollStft::getLabelAreaHeight() const {
    const juce::Font font = Common::getLabelFont();
    const float noteAreaHeight = juce::GlyphArrangement::getStringWidth(font, "Ww8") + 4.0f;
    const float hzAreaHeight = font.getHeight() + 4.0f;

    float totalHeight = 0.0f;
    if (processor.settings.isShowRollLabels()) totalHeight += noteAreaHeight;
    if (processor.settings.isRawMode) totalHeight += hzAreaHeight;
    return totalHeight;
}

float RollStft::getDbAxisWidth() const {
    if (!processor.settings.isRawMode) return 0.0f;
    const juce::Font dbFont = Common::getLabelFont().withHeight(dbLabelFontSize);
    return juce::GlyphArrangement::getStringWidth(dbFont, "-60") + dbLabelMarginRight + 4.0f;
}

float RollStft::freqToMidi(float freq) {
    if (freq <= 0.0f) return 0.0f;
    return 69.0f + 12.0f * std::log2(freq / 440.0f);
}

float RollStft::frequencyToX(float frequencyHz, float width, float xOffset) {
    const float midi = freqToMidi(frequencyHz);
    return xOffset + (width - xOffset) * ((midi - minMidiNote) / (maxMidiNote - minMidiNote));
}

void RollStft::paintTooltip(
    juce::Graphics& graphics,
    const int physicalWidth,
    const int physicalHeight,
    const juce::StringArray& tooltipLines
) {
    const float tooltipPadding = 6.0f;
    const juce::Font tooltipFont(juce::FontOptions(12.0f).withName(juce::Font::getDefaultMonospacedFontName()));
    const float tooltipLineHeight = std::ceil(tooltipFont.getHeight());
    float maxLineWidth = 0.0f;
    for (const auto& line : tooltipLines) {
        maxLineWidth = std::max(maxLineWidth, juce::GlyphArrangement::getStringWidth(tooltipFont, line));
    }
    maxLineWidth = std::ceil(maxLineWidth + 2.0f); // Add safety margin

    const float tooltipWidth = maxLineWidth + tooltipPadding * 2.0f;
    const float tooltipHeight = static_cast<float>(tooltipLines.size()) * tooltipLineHeight + tooltipPadding * 2.0f;

    // Offset tooltip so it doesn't cover the crosshair intersection
    float tooltipX = static_cast<float>(mousePosition.x) + 10.0f;
    float tooltipY = static_cast<float>(mousePosition.y) + 10.0f;

    // Flip tooltip if it goes off-screen
    if (tooltipX + tooltipWidth > static_cast<float>(physicalWidth)) {
        tooltipX = static_cast<float>(mousePosition.x) - tooltipWidth - 10.0f;
    }
    if (tooltipY + tooltipHeight > static_cast<float>(physicalHeight)) {
        tooltipY = static_cast<float>(mousePosition.y) - tooltipHeight - 10.0f;
    }

    // Background
    graphics.setColour(juce::Colours::black.withAlpha(0.6f));
    graphics.fillRoundedRectangle(tooltipX, tooltipY, tooltipWidth, tooltipHeight, 4.0f);
    graphics.setColour(juce::Colours::white.withAlpha(0.2f));
    graphics.drawRoundedRectangle(tooltipX, tooltipY, tooltipWidth, tooltipHeight, 4.0f, 1.0f);

    // Text
    graphics.setColour(juce::Colours::white);
    graphics.setFont(tooltipFont);
    for (int i = 0; i < tooltipLines.size(); ++i) {
        graphics.drawText(
            tooltipLines[i],
            juce::Rectangle<float>(
                tooltipX + tooltipPadding,
                tooltipY + tooltipPadding + static_cast<float>(i) * tooltipLineHeight,
                maxLineWidth,
                tooltipLineHeight
            ),
            juce::Justification::centredLeft,
            false
        );
    }
}

void RollStft::paintCrosshairs(
    juce::Graphics& graphics,
    const int physicalWidth,
    const int physicalHeight,
    const bool isHorizontal,
    const int logicalWidth,
    const int plotHeight
) {
    juce::StringArray tooltipLines;
    bool shouldShowTooltip = false;

    // Draw crosshairs at mouse position
    if (mousePosition.x >= 0 && mousePosition.y >= 0) {
        juce::Point<float> logicalMouse;
        if (isHorizontal) {
            logicalMouse = mousePosition.toFloat().transformedBy(cachedHorizontalTransformInverted);
        } else {
            logicalMouse = mousePosition.toFloat();
        }

        const float dbAxisWidth = getDbAxisWidth();

        if (logicalMouse.x >= dbAxisWidth && logicalMouse.x <= static_cast<float>(logicalWidth) &&
            logicalMouse.y >= 0.0f && logicalMouse.y <= static_cast<float>(plotHeight)) {

            // Draw crosshair lines using the correct transform
            {
                juce::Graphics::ScopedSaveState graphicsState(graphics);
                if (isHorizontal) {
                    graphics.addTransform(
                        cachedHorizontalTransform
                    );
                }
                graphics.setColour(juce::Colours::white.withAlpha(0.4f));
                graphics.drawLine(dbAxisWidth, logicalMouse.y, static_cast<float>(logicalWidth), logicalMouse.y, 1.0f);
                graphics.drawLine(logicalMouse.x, 0.0f, logicalMouse.x, static_cast<float>(plotHeight), 1.0f);
            }

            // Calculate tooltip values
            const float midi = (logicalMouse.x - dbAxisWidth) / (static_cast<float>(logicalWidth) - dbAxisWidth) * (
                maxMidiNote - minMidiNote) + minMidiNote;
            const float freq = 440.0f * std::pow(2.0f, (midi - 69.0f) / 12.0f);
            const float normalizedY = (static_cast<float>(plotHeight) - logicalMouse.y) / static_cast<float>(plotHeight);
            const float dbValue = normalizedY * 90.0f - 90.0f;

            const int wholeMidi = static_cast<int>(std::round(midi));
            const int roundedCents = static_cast<int>(std::round((midi - static_cast<float>(wholeMidi)) * 100.0f));
            const juce::String noteName = Tone::getNoteName(wholeMidi, processor.settings.isLetterNotation);
            const juce::String centsString = (roundedCents >= 0 ? "+" : "-") + juce::String(std::abs(roundedCents)).
                paddedLeft('0', 2) + "c";

            tooltipLines.add(noteName + " (" + centsString + ")");
            tooltipLines.add(juce::String(freq, 1) + " Hz");
            tooltipLines.add(juce::String(dbValue, 1) + " dB");
            shouldShowTooltip = true;
        }
    }

    if (shouldShowTooltip) {
        paintTooltip(graphics, physicalWidth, physicalHeight, tooltipLines);
    }
}

void RollStft::paint(juce::Graphics& graphics) {
    if (!cachedFrame.isValid()) {
        buildFrame();
    }

    const int physicalWidth = getWidth();
    const int physicalHeight = getHeight();
    const bool isHorizontal = processor.settings.isFlipRollHorizontal;
    const int logicalWidth = isHorizontal ? physicalHeight : physicalWidth;
    const int logicalHeight = isHorizontal ? physicalWidth : physicalHeight;
    const float labelAreaHeight = getLabelAreaHeight();
    const int plotHeight = std::max(1, logicalHeight - static_cast<int>(labelAreaHeight));

    {
        juce::Graphics::ScopedSaveState graphicsState(graphics);

        if (isHorizontal) {
            graphics.addTransform(
                cachedHorizontalTransform
            );
        }

        if (cachedFrame.isValid()) {
            graphics.drawImageAt(cachedFrame, 0, 0);
        }

        if (processor.settings.isShowSmoke) {
            const float dbAxisWidth = getDbAxisWidth();
            juce::Graphics::ScopedSaveState smokeGraphicsState(graphics);
            graphics.reduceClipRegion(
                static_cast<int>(dbAxisWidth),
                0,
                logicalWidth - static_cast<int>(dbAxisWidth),
                plotHeight
            );
            paintSmoke(graphics);
        }

        if (!activePeaks.empty() && processor.settings.isShowForrest) {
            paintForrest(graphics);
        }
    }

    paintCrosshairs(graphics, physicalWidth, physicalHeight, isHorizontal, logicalWidth, plotHeight);
}

void RollStft::buildFrame() {
    const bool isHorizontal = processor.settings.isFlipRollHorizontal;
    const int logicalWidth = isHorizontal ? getHeight() : getWidth();
    const int logicalHeight = isHorizontal ? getWidth() : getHeight();
    if (logicalWidth <= 0 || logicalHeight <= 0) return;

    // Create a transparent image (the 'true' flag clears it to zero alpha)
    cachedFrame = juce::Image(juce::Image::ARGB, logicalWidth, logicalHeight, true);
    juce::Graphics graphics(cachedFrame);

    const auto totalHeight = static_cast<float>(logicalHeight);
    const float labelAreaHeight = getLabelAreaHeight();
    const float dbAxisWidth = getDbAxisWidth();
    const float plotHeight = std::max(1.0f, totalHeight - labelAreaHeight);
    const juce::Font labelFont = Common::getLabelFont();
    graphics.setFont(labelFont);
    const float labelHeight = labelFont.getHeight();
    const float maxTextWidth = juce::GlyphArrangement::getStringWidth(labelFont, "Ww8");
    const int startMidi = static_cast<int>(ceil(minMidiNote));
    const int endMidi = static_cast<int>(floor(maxMidiNote));
    for (int midiNote = startMidi; midiNote <= endMidi; ++midiNote) {
        const int chroma = Common::fast_mod12(midiNote);

        // fixme: move to Tone
        // Identify standard "black" keys
        const bool isBlackKey = chroma == 1 || chroma == 3 || chroma == 6 || chroma == 8 || chroma == 10;

        const float hz = 440.0f * pow(2.0f, (static_cast<float>(midiNote) - 69.0f) / 12.0f);
        const float targetCenter = frequencyToX(hz, static_cast<float>(logicalWidth), dbAxisWidth);

        constexpr float startY = 0.0f;
        const float endY = plotHeight;

        const juce::Colour baseColor = Tone::chromaticScale[static_cast<size_t>(chroma)].color;
        const juce::Colour gridColor = juce::Colours::black.interpolatedWith(baseColor, 0.3f);
        graphics.setColour(gridColor);

        if (isBlackKey) {
            constexpr float dashLengths[] = {4.0f, 4.0f};
            graphics.drawDashedLine(juce::Line(targetCenter, startY, targetCenter, endY), dashLengths, juce::numElementsInArray(dashLengths), 1.0f);
        } else {
            graphics.drawLine(targetCenter, startY, targetCenter, endY, 1.0f);
        }

        if (processor.settings.isShowRollLabels()) {
            paintLabel(
                graphics,
                labelHeight,
                maxTextWidth,
                midiNote,
                targetCenter,
                totalHeight,
                baseColor,
                isHorizontal
            );
        }
    }

    paintRawAxisLabels(graphics, plotHeight, static_cast<float>(logicalWidth));
}

void RollStft::paintHzAxis(
    juce::Graphics& graphics,
    float logicalWidth,
    const juce::Font& labelFont,
    const float labelHeight,
    const float totalHeight,
    const float dbAxisWidth
) const {
    const float noteAreaHeight = processor.settings.isShowRollLabels()
                                     ? juce::GlyphArrangement::getStringWidth(labelFont, "Ww8") + 4.0f
                                     : 0.0f;
    const float hzStartY = totalHeight - noteAreaHeight;

    for (float hz : hzValues) {
        const float targetCenter = frequencyToX(hz, logicalWidth, dbAxisWidth);
        if (targetCenter < 0 || targetCenter > logicalWidth) continue;

        juce::String labelText;
        if (hz >= 1000.0f) {
            labelText = juce::String(static_cast<int>(hz / 1000.0f)) + "k";
        } else {
            labelText = juce::String(static_cast<int>(hz));
        }

        paintHzLabel(graphics, labelHeight, labelText, targetCenter, hzStartY, juce::Colours::grey);
    }
}

void RollStft::paintDbAxis(
    juce::Graphics& graphics,
    float plotHeight,
    const bool isHorizontal,
    const juce::Font& labelFont,
    const float dbAxisWidth
) {
    graphics.setColour(juce::Colours::grey);
    graphics.setFont(labelFont.withHeight(dbLabelFontSize));
    for (int db : dbValues) {
        const float norm = (static_cast<float>(db) + 90.0f) / 90.0f;
        const float y = plotHeight * (1.0f - norm);

        if (isHorizontal) {
            juce::Graphics::ScopedSaveState scopedState(graphics);
            const float centerX = dbAxisWidth * 0.5f;
            graphics.addTransform(juce::AffineTransform::rotation(juce::MathConstants<float>::halfPi, centerX, y));
            // Rotate 90 CW around (centerX, y) to keep text upright in the final view.
            // The box is centered on (centerX, y) in rotated logical space.
            graphics.drawText(
                juce::String(db),
                juce::Rectangle<float>(centerX - dbLabelHeight, y - dbAxisWidth * 0.5f, dbLabelHeight * 2.0f, dbAxisWidth),
                juce::Justification::centred,
                false
            );
        } else {
            graphics.drawText(
                juce::String(db),
                juce::Rectangle<float>(0.0f, y - dbLabelHeight * 0.5f, dbAxisWidth - dbLabelMarginRight, dbLabelHeight),
                juce::Justification::centredRight,
                false
            );
        }
    }
}

void RollStft::paintRawAxisLabels(juce::Graphics& graphics, float plotHeight, float logicalWidth) {
    if (!processor.settings.isRawMode) return;

    const bool isHorizontal = processor.settings.isFlipRollHorizontal;
    const juce::Font labelFont = Common::getLabelFont();
    const float labelHeight = labelFont.getHeight();
    const float totalHeight = static_cast<float>(graphics.getClipBounds().getHeight());
    const float dbAxisWidth = getDbAxisWidth();

    paintHzAxis(graphics, logicalWidth, labelFont, labelHeight, totalHeight, dbAxisWidth);

    paintDbAxis(graphics, plotHeight, isHorizontal, labelFont, dbAxisWidth);
}

void RollStft::paintLabel(
    juce::Graphics& graphics,
    const float labelHeight,
    const float maxTextWidth,
    const int midiNote,
    const float targetCenter,
    const float startY,
    const juce::Colour baseColor,
    const bool isHorizontal
) const {
    if (midiNote == minMidiNote || midiNote == maxMidiNote) {
        // Not drawing a half label
        return;
    }

    const juce::String name = Tone::getNoteName(midiNote, processor.settings.isLetterNotation);

    graphics.setColour(baseColor);
    juce::Graphics::ScopedSaveState graphicsState(graphics);

    if (isHorizontal) {
        const float rotX = targetCenter + labelHeight / 2.0f;
        const float rotY = startY - maxTextWidth;
        graphics.addTransform(
            juce::AffineTransform::rotation(juce::MathConstants<float>::halfPi, rotX, rotY)
        );
        graphics.drawText(
            name,
            juce::Rectangle(rotX, rotY, maxTextWidth, labelHeight),
            juce::Justification::centredLeft,
            false
        );
    } else {
        graphics.addTransform(
            juce::AffineTransform::rotation(-juce::MathConstants<float>::halfPi, targetCenter, startY - 2.0f)
        );
        graphics.drawText(
            name,
            juce::Rectangle<float>(targetCenter, startY - 2.0f - labelHeight / 2.0f, maxTextWidth, labelHeight),
            juce::Justification::centredLeft,
            false
        );
    }
}

void RollStft::paintHzLabel(
    juce::Graphics& graphics,
    const float labelHeight,
    const juce::String& text,
    const float targetCenter,
    const float startY,
    const juce::Colour color
) {
    graphics.setColour(color);

    const float textWidth = juce::GlyphArrangement::getStringWidth(graphics.getCurrentFont(), text);

    // Hz labels are parallel to the frequency axis (Logical X).
    // In logical space, they are always horizontal and sit in a strip of height labelHeight.
    // Placement: Centered on targetCenter, anchored to the bottom of the strip (startY).
    graphics.drawText(
        text,
        juce::Rectangle(targetCenter - textWidth / 2.0f, startY - labelHeight, textWidth, labelHeight),
        juce::Justification::centredBottom,
        false
    );
}

void RollStft::paintForrest(juce::Graphics& graphics) const {
    const bool isHorizontal = processor.settings.isFlipRollHorizontal;
    const int logicalWidth = isHorizontal ? getHeight() : getWidth();
    const int logicalHeight = isHorizontal ? getWidth() : getHeight();

    const int width = logicalWidth;
    const float plotHeight = std::max(1.0f, static_cast<float>(logicalHeight) - getLabelAreaHeight());
    const float dbAxisWidth = getDbAxisWidth();

    for (const auto& peak : activePeaks) {
        const float xPos = frequencyToX(peak.frequencyHz, static_cast<float>(width), dbAxisWidth);

        if (xPos >= dbAxisWidth && xPos <= static_cast<float>(width)) {
            // Configurable razor-sharp stems for the Forrest
            float stemWidthPixels = 5.0f;
            const bool doDynamicStem = enableDynamicStemWidth && !processor.settings.isRawMode;
            if (doDynamicStem) {
                const float nextX = frequencyToX(
                    peak.frequencyHz + peak.bandwidthHz,
                    static_cast<float>(width),
                    dbAxisWidth
                );
                // +1.0f forces deliberate sub-pixel overlap to completely kill rendering gaps
                stemWidthPixels = std::max(1.0f, nextX - xPos + 1.0f);
            }

            const float normalizedMagnitude = std::min(1.0f, std::max(0.0f, peak.magnitude));
            const auto barHeight = normalizedMagnitude * plotHeight;

            const float midi = freqToMidi(peak.frequencyHz);
            const float continuousChroma = Common::fast_fmod12(midi);

            const juce::Colour color = Tone::getContinuousColor(continuousChroma);
            graphics.setColour(color);

            graphics.fillRoundedRectangle(
                xPos - stemWidthPixels * 0.5f,
                plotHeight - barHeight,
                stemWidthPixels,
                barHeight,
                2.0f
            );
        }
    }
}

void RollStft::pumpSmoke() {
    if (activePeaks.empty() || !smokeImage.isValid()) {
        return;
    }

    const bool isHorizontal = processor.settings.isFlipRollHorizontal;
    const int logicalWidth = isHorizontal ? getHeight() : getWidth();
    const int logicalHeight = isHorizontal ? getWidth() : getHeight();

    const int width = logicalWidth;
    const int height = std::max(1, logicalHeight - static_cast<int>(getLabelAreaHeight()));
    if (width <= 0 || height <= 0) return;

    constexpr int speedPx = static_cast<int>(smokeSpeedPxPerFrame);

    // Advance the scroll offset and wrap it like a treadmill
    smokeScrollOffset += speedPx;
    if (smokeScrollOffset >= height) smokeScrollOffset -= height;

    // Calculate where in the image memory the new row should be drawn
    int drawY = smokeScrollOffset - speedPx;
    if (drawY < 0) drawY += height;

    // Clear the new row first to prevent ghosting from previous treadmill cycles
    smokeImage.clear(juce::Rectangle<int>(0, drawY, width, speedPx), juce::Colours::transparentBlack);

    juce::Graphics graphics(smokeImage);

    const float sampleRate = processor.getSampleRate() > 0.0 ? static_cast<float>(processor.getSampleRate()) : 44100.0f;
    const float binResolutionHz = sampleRate / 32768.0f;
    const auto fWidth = static_cast<float>(width);
    const float dbAxisWidth = getDbAxisWidth();
    const float effectiveWidth = fWidth - dbAxisWidth;
    constexpr float midiRangeInv = 1.0f / (maxMidiNote - minMidiNote);
    const bool doDynamicStem = enableDynamicStemWidth && !processor.settings.isRawMode;

    // Extreme Math Optimization: Pre-calculate the derivative of the MIDI scale
    // to bypass calling std::log2 multiple times per peak.
    const float derivativeFactor = effectiveWidth * midiRangeInv * 17.3123405f * binResolutionHz;

    for (const auto& peak : activePeaks) {
        if (peak.magnitude > 0.05f) {
            // Prevents rendering absolute silence noise

            // Inline the math for the primary position calculation
            const float midi = 69.0f + 12.0f * std::log2(peak.frequencyHz / 440.0f);
            const float xPos = dbAxisWidth + effectiveWidth * ((midi - minMidiNote) * midiRangeInv);

            // Fast bounds culling
            if (xPos >= dbAxisWidth - 10.0f && xPos <= fWidth + 10.0f) {
                // Configurable razor-sharp stems for the Smoke
                float stemWidthPixels = 4.0f;
                if (doDynamicStem) {
                    // Use the fast derivative approximation instead of another heavy log2
                    const float nextX = xPos + derivativeFactor / peak.frequencyHz;
                    // +1.0f forces deliberate sub-pixel overlap to completely kill rendering gaps
                    stemWidthPixels = std::max(1.0f, nextX - xPos + 1.0f);
                }

                // Fast continuous modulus (replaces heavy std::fmod)
                const float continuousChroma = Common::fast_fmod12(midi);

                const juce::Colour baseColor = Tone::getContinuousColor(continuousChroma);
                constexpr float undimmingGain = 1.3f;

                const float clampedMag = std::min(1.0f, peak.magnitude * undimmingGain);
                const juce::Colour color = juce::Colours::black.interpolatedWith(baseColor, clampedMag);

                graphics.setColour(color);
                graphics.fillRect(
                    xPos - stemWidthPixels * 0.5f,
                    static_cast<float>(drawY),
                    stemWidthPixels,
                    static_cast<float>(speedPx)
                );
            }
        }
    }
}

void RollStft::paintSmoke(const juce::Graphics& graphics) const {
    if (!smokeImage.isValid()) return;

    const bool isHorizontal = processor.settings.isFlipRollHorizontal;
    const int logicalHeight = isHorizontal ? getWidth() : getHeight();

    const int height = std::max(1, logicalHeight - static_cast<int>(getLabelAreaHeight()));

    // Draw the two halves of the ring buffer to create a flawless infinite upward scroll
    graphics.drawImageAt(smokeImage, 0, -smokeScrollOffset);
    graphics.drawImageAt(smokeImage, 0, height - smokeScrollOffset);
}
