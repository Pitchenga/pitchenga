cmake_minimum_required(VERSION 3.22)
project(Pitchenga VERSION 1.0.0)

include(FetchContent)
FetchContent_Declare(
  eigen
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG        3.4.0
)
FetchContent_MakeAvailable(eigen)
FetchContent_Declare(
        pitch_detector
        GIT_REPOSITORY https://github.com/adamski/pitch_detector.git
        GIT_TAG        d3b6097
        SOURCE_DIR     ${CMAKE_BINARY_DIR}/_deps/pitch_detector
)
FetchContent_Declare(
        audio_fft
        GIT_REPOSITORY https://github.com/adamski/audio_fft.git
        GIT_TAG        922b30a
        SOURCE_DIR     ${CMAKE_BINARY_DIR}/_deps/audio_fft
)
FetchContent_MakeAvailable(pitch_detector audio_fft)

# 1. Tell CMake where to find the JUCE framework
add_subdirectory(JUCE)

juce_add_module(${CMAKE_BINARY_DIR}/_deps/pitch_detector)
juce_add_module(${CMAKE_BINARY_DIR}/_deps/audio_fft)

# 2. Define the exact plugin we are building
juce_add_plugin(Pitchenga
        # Plugin metadata
        PLUGIN_MANUFACTURER_CODE "Ptng"  # Exactly 4 chars
        PLUGIN_CODE "Ptng"               # Exactly 4 chars
        FORMATS AU Standalone            # The magic dual-compile flag
        PRODUCT_NAME "Pitchenga"

        # Architecture rules
        IS_SYNTH FALSE                   # It's an effect/visualizer, not a synth generator
        NEEDS_MIDI_INPUT TRUE            # We need MIDI for your Wurlitzer/Keyboard
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS FALSE
        COPY_PLUGIN_AFTER_BUILD TRUE      # Automatically drops the AU into your Mac's plugin folder
        MICROPHONE_PERMISSION_ENABLED TRUE)    # This ensures the Info.plist has the usage description for macOS TCC

# 3. Tell CMake which C++ files to compile
target_sources(Pitchenga PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/CqtEngine.h
        Source/CqtEngine.cpp
        Source/Analyzers.h
        Source/Analyzers.cpp
        Source/ColorPalette.h
        Source/ColorPalette.cpp
        Source/TunerViz.h
        Source/TunerViz.cpp
        Source/CircleViz.h
        Source/CircleViz.cpp
        Source/LineViz.h
        Source/LineViz.cpp
        Source/MathWorker.h
        Source/MathWorker.cpp
        Source/Control.h
        Source/Control.cpp
)

# 4. Link the heavy-lifting JUCE modules (Audio, GUI, and Hardware-Accelerated Math)
target_link_libraries(Pitchenga PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp                 # Contains the fast FFT class
        Eigen3::Eigen
        pitch_detector
        audio_fft
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags)

target_compile_features(Pitchenga PUBLIC cxx_std_20)
