#include "Splitter.h"
#include "Needle.h"

Splitter::Splitter(PitchengaAudioProcessor& proc) : processor(proc) {
    setMouseCursor(juce::MouseCursor::UpDownResizeCursor);
}

void Splitter::mouseEnter(const juce::MouseEvent&) {
    isHovered = true;
    if (processor.settings.isLayoutHorizontal) {
        setMouseCursor(juce::MouseCursor::LeftRightResizeCursor);
    } else {
        setMouseCursor(juce::MouseCursor::UpDownResizeCursor);
    }
    repaint();
}

void Splitter::mouseExit(const juce::MouseEvent&) {
    isHovered = false;
    repaint();
}

void Splitter::paint(juce::Graphics& graphics) {
    if (isHovered) {
        graphics.fillAll(juce::Colours::darkgrey);
    } else {
        graphics.fillAll(juce::Colours::black);
    }
}

void Splitter::mouseDrag(const juce::MouseEvent& event) {
    if (auto* parent = getParentComponent()) {
        auto eventInParent = event.getEventRelativeTo(parent);

        if (processor.settings.isLayoutHorizontal) {
            const float availableWidth = static_cast<float>(parent->getWidth());
            if (availableWidth > 4.0f) {
                // In horizontal mode, [Eye/Needle] is on the left
                // eyeWidth = availableWidth * (1.0 - splitRatio)
                // So adjustedX = eyeWidth => adjustedX / availableWidth = 1.0 - splitRatio
                // splitRatio = 1.0 - (adjustedX / availableWidth)
                const float newRatio = 1.0f - (static_cast<float>(eventInParent.x) / (availableWidth - 4.0f));
                const float clampedRatio = std::max(0.1f, std::min(0.9f, newRatio));
                if (std::abs(processor.settings.splitRatio - clampedRatio) > 0.001f) {
                    processor.settings.splitRatio = clampedRatio;
                    if (onDragged) onDragged();
                }
            }
        } else {
            const float topControlHeight = processor.settings.isShowTweakPanel ? 48.0f : 24.0f; // fixme: use preferredHeight
            const float bottomNeedleHeight = processor.settings.isShowNeedle ? Needle::getPreferredHeight() + 1.0f : 0.0f;
            const float availableHeight = static_cast<float>(parent->getHeight()) - topControlHeight - bottomNeedleHeight - 4.0f;

            const float adjustedY = static_cast<float>(eventInParent.y) - topControlHeight;

            if (availableHeight > 0.0f) {
                const float newRatio = adjustedY / availableHeight;
                // Prevent dragging completely out of bounds (keep between 10% and 90%)
                const float clampedRatio = std::max(0.1f, std::min(0.9f, newRatio));

                if (std::abs(processor.settings.splitRatio - clampedRatio) > 0.001f) {
                    processor.settings.splitRatio = clampedRatio;
                    if (onDragged) onDragged();
                }
            }
        }
    }
}
