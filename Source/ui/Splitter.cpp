#include "Splitter.h"
#include "Needle.h"

Splitter::Splitter(PitchengaAudioProcessor& proc) : processor(proc) {
    setMouseCursor(juce::MouseCursor::UpDownResizeCursor);
}

void Splitter::mouseEnter(const juce::MouseEvent&) {
    isHovered = true;
    repaint();
}

void Splitter::mouseExit(const juce::MouseEvent&) {
    isHovered = false;
    repaint();
}

void Splitter::paint(juce::Graphics& g) {
    if (isHovered) {
        g.fillAll(juce::Colours::white.withAlpha(0.2f));
    } else {
        g.fillAll(juce::Colours::grey.withAlpha(0.2f));
    }
}

void Splitter::mouseDrag(const juce::MouseEvent& e) {
    if (auto* parent = getParentComponent()) {
        auto eventInParent = e.getEventRelativeTo(parent);

        // Calculate dynamic bounds exactly matching resized() architecture
        const float topControlHeight = 24.0f;
        const float bottomNeedleHeight = processor.settings.showNeedle ? Needle::getPreferredHeight() + 1.0f : 0.0f;
        const float availableHeight = static_cast<float>(parent->getHeight()) - topControlHeight - bottomNeedleHeight;

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