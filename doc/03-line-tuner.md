
Specification: LineTuner Component
==================================

1\. Component Overview
----------------------

The `LineTuner` is a standalone, horizontal 2D `juce::Component` that acts as a continuous, high-resolution digital linear tuner. It serves as the direct, modernized replacement for the legacy `JSlider` tuner implementation.

2\. Architectural & DSP Constraints
-----------------------------------

*   **Decoupling:** The `LineTuner` must be strictly decoupled from the circular clock visualizer and any future waterfall visualizers. They do not share rendering loops or state.
*   **Shared Color Map:** The existing chromatic array must be extracted into a shared static utility (e.g., `ColorPalette`). Both visualizers will read from this single source of truth.
*   **DSP Agnosticism:** The component performs zero frequency analysis itself. It expects to be fed a highly accurate `float` frequency from the audio thread (calculated via an MPM algorithm module) using a thread-safe setter.

3\. UI & Layout Requirements
----------------------------

*   **Dimensions:** A horizontal strip stretching across the app screen, with the height of a typical desktop scrollbar (approx. 24px).
*   **The Strip:** The component consists entirely of a continuous (non-binned) chromatic gradient precisely mapped to the configured pitch range.
*   **Base State (Dimming):** The entire strip is pre-calculated at a dimmed state of **30% brightness**. This dimming MUST be achieved mathematically via **interpolation with black**, NOT by using alpha/opacity transparency.

4\. Labels & Markers
--------------------

*   **Base State (Static):** The strip features vertical text labels denoting the names of the perfect whole notes, paired with a **1x5 pixel vertical tick** pointing to the exact geometric center of that pitch. These labels and ticks are also pre-dimmed to **30% brightness** using interpolation with black.
*   **Active State (Illumination):** When a valid pitch is detected, the `LineTuner` identifies which perfect whole note the pitch "belongs to" (the nearest pitch class). It redraws _only_ that specific label and tick at **100% brightness** in its exact chromatic color.
*   **The Pitch Indicator:** Exactly one pixel-wide vertical line on the strip is illuminated to **100% brightness** at the precise float coordinate of the detected frequency.

5\. Performance & Rendering Optimization
----------------------------------------

*   **Pre-calculation:** Calculating the static base gradient, ticks, and labels per pixel during the 30 FPS `paint()` loop is strictly forbidden.
*   **Image Caching:** The 30% dimmed gradient, along with all 30% dimmed ticks and labels, must be pre-rendered and baked into a single 2D `juce::Image` when the component is initialized or resized.
*   **The Paint Loop:** The `paint()` loop must simply:
   1.  Draw the baked `juce::Image` opaquely.
   2.  Draw the single illuminated label and tick at 100% brightness.
   3.  Draw the single active pitch indicator line at 100% brightness.

6\. Configuration & Data Ranges
-------------------------------

*   **Dynamic Range:** The minimum and maximum bounds must be configurable via a setter, which triggers a recalculation of the cached `juce::Image`.
*   **Range Defaults:** The default visible range of the strip spans from **La0** (A0 - MIDI 21, 27.5 Hz) to **Mi4** (E4 - MIDI 64, 329.63 Hz).

