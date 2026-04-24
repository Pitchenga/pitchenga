cmake_minimum_required(VERSION 3.22)
project(Pitchenga VERSION 1.0.0)

# Auto-initialize Git submodules if they are missing
find_package(Git QUIET)
if (GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    if (NOT EXISTS "${PROJECT_SOURCE_DIR}/libs/JUCE/CMakeLists.txt")
        message(STATUS "Submodules not found! Automatically running: git submodule update --init --recursive")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if (NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed. Please checkout submodules manually.")
        endif ()
    endif ()
endif ()

# Framework and Dependencies (submoduled in /libs, see .gitmodules)

# JUCE
add_subdirectory(libs/JUCE)

# Eigen (Math library)
add_subdirectory(libs/eigen)

# Pitch Detection and FFT helpers
# These require being added as JUCE modules to integrate correctly with the plugin
juce_add_module(libs/pitch_detector)
juce_add_module(libs/audio_fft)

# Define the exact plugin we are building
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

# Tell CMake which C++ files to compile
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
        Source/ui/Needle.h
        Source/ui/Needle.cpp
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

# Generate a build timestamp
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/build_timestamp.h
        COMMAND ${CMAKE_COMMAND} -DOUTPUT_FILE=${CMAKE_CURRENT_BINARY_DIR}/build_timestamp.h -P ${CMAKE_CURRENT_SOURCE_DIR}/generate_timestamp.cmake
        DEPENDS ${PITCHENGA_SOURCES}
)

target_sources(Pitchenga PRIVATE
        ${PITCHENGA_SOURCES}
        ${CMAKE_CURRENT_BINARY_DIR}/build_timestamp.h
)

# Link the libraries
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
