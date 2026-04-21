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
set(PITCHENGA_SOURCES
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/math/Cqt.h
        Source/math/Cqt.cpp
        Source/math/Analyzers.h
        Source/math/Analyzers.cpp
        Source/Tone.h
        Source/Tone.cpp
        Source/ui/Tuna.h
        Source/ui/Tuna.cpp
        Source/ui/Eye.h
        Source/ui/Eye.cpp
        Source/ui/RollStft.h
        Source/ui/RollStft.cpp
        Source/ui/RollCqt.h
        Source/ui/RollCqt.cpp
        Source/math/Math.h
        Source/math/Math.cpp
        Source/ui/Control.h
        Source/ui/Control.cpp
        Source/math/Agc.h
        Source/math/Agc.cpp
        Source/ui/Splitter.h
        Source/ui/Splitter.cpp
        Source/Util.h
        Source/Util.cpp
        Source/math/Stft.h
        Source/math/Stft.cpp
)

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/build_timestamp.h
        COMMAND ${CMAKE_COMMAND} -DOUTPUT_FILE=${CMAKE_CURRENT_BINARY_DIR}/build_timestamp.h -P ${CMAKE_CURRENT_SOURCE_DIR}/generate_timestamp.cmake
        DEPENDS ${PITCHENGA_SOURCES}
)

target_sources(Pitchenga PRIVATE
        ${PITCHENGA_SOURCES}
        ${CMAKE_CURRENT_BINARY_DIR}/build_timestamp.h
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

target_compile_definitions(Pitchenga PRIVATE CMAKE_BUILD_PROFILE=$<CONFIG>)

# Make generated timestamp header available to the build
target_include_directories(Pitchenga PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
