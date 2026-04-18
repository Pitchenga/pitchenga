1\. Component Overview
----------------------

The `LineVisualizer` is a JUCE `Component` that renders the magnitude of all analyzed pitch bins across the active
octaves. It presents the spectrum linearly, functioning as a horizontal frequency-domain UI element.

* **Primary Data Source:** Spectral magnitude data provided by the `CqtEngine`.
* **Coordinate System:** \* **X-axis:** Pitch bins (unfolded from the lowest to the highest frequency).
    * **Y-axis:** Bin magnitude/velocity (height from the bottom).
* **Visual Style:** Vertical bars with chroma-based coloring.

* * *

2\. Technical Requirements
--------------------------

### A. Data Integration & Auto-Adaptation

* **Dynamic Sizing:** The visualizer must not hardcode the number of bins. On every update cycle, it must dynamically
  query the `CqtEngine` for its current configuration (`getOctaves()` and `getSemitonesPerOctave()`). It will
  automatically adapt its drawing logic to match these settings.
* **Buffer Extraction:** The component must fetch the current magnitude array from the engine.
* **Update Frequency:** The component should repaint at approximately 60 FPS using a `Timer` to ensure smooth visual
  movement.

### B. Visual Mapping Logic

| Attribute      | Logic                                                       | Implementation Note                                                        |
|----------------|-------------------------------------------------------------|----------------------------------------------------------------------------|
| **X-Position** | `binIndex * (componentWidth / totalBins)`                   | Linear distribution across the width.                                      |
| **Height**     | `normalizedMagnitude * componentHeight`                     | Magnitude mapped from logarithmic decibel scale to a linear 0.0–1.0 range. |
| **Color**      | `ColorPalette::getColorForChroma(binIndex % binsPerOctave)` | Uses the existing chroma-to-color mapping logic.                           |

* * *

3\. Implementation Details
--------------------------

### `LineVisualizer.h` Structure

```
class LineVisualizer : public juce::Component, private juce::Timer
{
public:
    LineVisualizer(PitchengaAudioProcessor&);
    void paint(juce::Graphics& g) override;
    void timerCallback() override;

private:
    PitchengaAudioProcessor& processor;
    std::vector<float> displayMagnitudes; 
    
    // Auto-adapting state variables
    int currentTotalBins = 0;
    int currentBinsPerOctave = 0;
};
```

### Rendering Logic (`paint` method)

1. **Clear Background:** Fill with a solid dark background color.
2. **Fetch Engine State:** Update `currentTotalBins` and `currentBinsPerOctave` directly from the `CqtEngine`.
3. **Iterate Bins:** Loop through all available bins in the unfolded spectrum.
4. **Calculate Bounds:**
    * Width per bar: `totalWidth / currentTotalBins`.
    * Height: Use a scaling multiplier to make weak signals visible.
5. **Draw Bars:**
    * Set the color based on the bin's chroma using the modulo operator.
    * Draw a filled rectangle (`juce::Graphics::fillRect`) originating from the bottom boundary.

* * *

4\. Layout Integration
----------------------

The `LineVisualizer` must be integrated into `PluginEditor.cpp` directly below the existing tuner.

**In `PluginEditor::resized()`:**

```
void PitchengaAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    
    // 1. LineTuner takes the top portion
    lineTunerComponent.setBounds(area.removeFromTop(100)); 
    
    // 2. LineVisualizer sits directly below the tuner
    lineVisualizerComponent.setBounds(area.removeFromTop(150));
    
    // 3. Remaining area for the circular visualizer
    cqtVisualizerComponent.setBounds(area);
}
```

* * *

5\. Visual Constraints
----------------------

* **Zero-Baseline:** All bars must originate exactly from the bottom `y = height` coordinate of the component, extending
  upward.


