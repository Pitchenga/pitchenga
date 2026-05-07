cmake_minimum_required(VERSION 3.22)
project(Pitchenga VERSION 0507.16.32)

# Calculate a dynamic version for local development to ensure the AU bundle
# shows more than "1.0.0" in hosts like Logic Pro.
if (NOT PITCHENGA_VERSION)
    set(PITCHENGA_VERSION ${PROJECT_VERSION})
    if (PROJECT_VERSION STREQUAL "1.0.0")
        execute_process(
                COMMAND bash "${CMAKE_CURRENT_SOURCE_DIR}/version.sh"
                OUTPUT_VARIABLE DYNAMIC_VERSION
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if (NOT DYNAMIC_VERSION STREQUAL "")
            set(PITCHENGA_VERSION ${DYNAMIC_VERSION})
        endif()
    endif()
endif()

if (APPLE)
    # Force macOS to build a Universal Binary (arm64 + x86_64) to support both Apple Silicon and Intel plugins
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "Build Universal Binaries" FORCE)

    # Generate the required entitlements file for hosting third-party plugins in a Hardened Runtime
    set(ENTITLEMENTS_FILE "${CMAKE_CURRENT_BINARY_DIR}/Pitchenga.entitlements")
    file(WRITE "${ENTITLEMENTS_FILE}"
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>
<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">
<plist version=\"1.0\">
<dict>
    <key>com.apple.security.device.audio-input</key>
    <true/>
    <key>com.apple.security.cs.disable-library-validation</key>
    <true/>
    <key>com.apple.security.cs.allow-jit</key>
    <true/>
    <key>com.apple.security.cs.allow-unsigned-executable-memory</key>
    <true/>
    <key>com.apple.security.cs.disable-executable-page-protection</key>
    <true/>
</dict>
</plist>
")
endif()

# Force static linking of the MSVC runtime to prevent "MSVCP140.dll not found" errors on Windows
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

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

# Define the exact plugin we are building
juce_add_plugin(Pitchenga
        VERSION "${PITCHENGA_VERSION}"
        COMPANY_NAME "Pitchenga"
        BUNDLE_ID "com.github.pitchenga.Pitchenga"
        PLUGIN_MANUFACTURER_CODE "Ptng"  # Exactly 4 chars
        PLUGIN_CODE "Ptng"               # Exactly 4 chars
        FORMATS AU VST3 Standalone       # The magic dual-compile flag
        PRODUCT_NAME "Pitchenga"
        ICON_BIG "Source/Assets/pitchenga-squircle-1024.png"
        ICON_SMALL "Source/Assets/pitchenga-squircle-256.png"

        # Architecture rules
        IS_SYNTH FALSE                   # It's an effect/visualizer, not a synth generator
        NEEDS_MIDI_INPUT TRUE            # We need MIDI for your Wurlitzer/Keyboard
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS FALSE
        COPY_PLUGIN_AFTER_BUILD TRUE      # Automatically drops the AU into your Mac's plugin folder
        MICROPHONE_PERMISSION_ENABLED TRUE
        MICROPHONE_PERMISSION_TEXT "Pitchenga needs microphone access to visualize your audio."
        PLUGINHOST_AU TRUE
        PLUGINHOST_VST3 TRUE
        PLIST_TO_MERGE "<plist><dict><key>NSAudioCaptureUsageDescription</key><string>Pitchenga needs system audio access to visualize your desktop audio.</string></dict></plist>")

# Explicitly separate the bundle identifiers to prevent LaunchServices collisions
if (TARGET Pitchenga_Standalone)
    set_target_properties(Pitchenga_Standalone PROPERTIES
            MACOSX_BUNDLE_GUI_IDENTIFIER "com.github.pitchenga.Pitchenga"
            XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.github.pitchenga.Pitchenga")
endif()

if (TARGET Pitchenga_AU)
    set_target_properties(Pitchenga_AU PROPERTIES
            MACOSX_BUNDLE_GUI_IDENTIFIER "com.github.pitchenga.Pitchenga.AU"
            XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.github.pitchenga.Pitchenga.AU")
endif()

if (TARGET Pitchenga_VST3)
    set_target_properties(Pitchenga_VST3 PROPERTIES
            MACOSX_BUNDLE_GUI_IDENTIFIER "com.github.pitchenga.Pitchenga.VST3"
            XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.github.pitchenga.Pitchenga.VST3")
endif()

if (APPLE)
    # Force unique Bundle IDs to prevent LaunchServices collisions between formats
    foreach(target_suffix AU VST3)
        if (TARGET Pitchenga_${target_suffix})
            add_custom_command(TARGET Pitchenga_${target_suffix} POST_BUILD
                    COMMAND /usr/libexec/PlistBuddy -c "Set :CFBundleIdentifier com.github.pitchenga.Pitchenga.${target_suffix}" "$<TARGET_BUNDLE_CONTENT_DIR:Pitchenga_${target_suffix}>/Info.plist"
                    COMMENT "Isolating ${target_suffix} Bundle ID"
                    VERBATIM)
        endif()
    endforeach()

    # For local development (1.0.0), inject a fresh timestamp-based version on every build.
    # This prevents hosts like Logic Pro from caching old AU versions.
    if (PROJECT_VERSION STREQUAL "1.0.0")
        foreach(target_suffix Standalone AU VST3)
            if (TARGET Pitchenga_${target_suffix})
                add_custom_command(TARGET Pitchenga_${target_suffix} POST_BUILD
                        COMMAND /bin/bash -c "V=$(bash ${CMAKE_CURRENT_SOURCE_DIR}/version.sh); /usr/libexec/PlistBuddy -c \"Set :CFBundleVersion $V\" \"$<TARGET_BUNDLE_CONTENT_DIR:Pitchenga_${target_suffix}>/Info.plist\"; /usr/libexec/PlistBuddy -c \"Set :CFBundleShortVersionString $V\" \"$<TARGET_BUNDLE_CONTENT_DIR:Pitchenga_${target_suffix}>/Info.plist\""
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        COMMENT "Injecting dynamic version into ${target_suffix} bundle"
                        VERBATIM)
            endif()
        endforeach()
    endif()
endif()

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
        Source/math/PitchDetector.h
        Source/DesktopAudioCapture.h
)

if (PROJECT_VERSION STREQUAL "1.0.0")
    # Generate dynamic timestamp for local dev on every build
    add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/version.h
            COMMAND ${CMAKE_COMMAND} -DOUTPUT_FILE=${CMAKE_CURRENT_BINARY_DIR}/version.h -P ${CMAKE_CURRENT_SOURCE_DIR}/version.cmake
            DEPENDS ${PITCHENGA_SOURCES}
    )
else()
    # Ensure the UI matches the Release Tag exactly for production builds
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/version.h" "#pragma once\n#define VERSION \"${PROJECT_VERSION}\"\n")
endif()

target_sources(Pitchenga PRIVATE
        ${PITCHENGA_SOURCES}
        ${CMAKE_CURRENT_BINARY_DIR}/version.h
)

# Link the libraries
target_link_libraries(Pitchenga PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp                 # Contains the fast FFT class
        Eigen3::Eigen
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags)

if (APPLE)
    target_link_libraries(Pitchenga PRIVATE
            "-framework CoreMedia"
            "-framework AVFoundation")

    if (TARGET Pitchenga_Standalone)
        target_link_libraries(Pitchenga_Standalone PRIVATE "-framework ScreenCaptureKit")
        target_sources(Pitchenga_Standalone PRIVATE Source/DesktopAudioCapture.mm)
        target_compile_definitions(Pitchenga_Standalone PRIVATE PITCHENGA_USE_SCREEN_CAPTURE=1)

        # Force signing the Standalone app with stable developer ID and custom plugin-host entitlements
        if (NOT DEFINED ENV{CI})
            add_custom_command(TARGET Pitchenga_Standalone POST_BUILD
                COMMAND /usr/bin/codesign --force --deep --options runtime --sign "Apple Development" --entitlements "${ENTITLEMENTS_FILE}" --preserve-metadata=identifier "$<TARGET_BUNDLE_DIR:Pitchenga_Standalone>"
                VERBATIM)

            # Load copy destination from .env if it exists
            set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ".env")
            if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.env")
                file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/.env" ENV_STRINGS)
                foreach(ENV_STRING ${ENV_STRINGS})
                    if (ENV_STRING MATCHES "^COPY_APP_TO=(.*)$")
                        string(CONFIGURE "${CMAKE_MATCH_1}" COPY_APP_TO)
                    endif()
                endforeach()
            endif()
            if (COPY_APP_TO AND NOT COPY_APP_TO STREQUAL "")
                add_custom_command(TARGET Pitchenga_Standalone POST_BUILD
                    COMMAND mkdir -p "${COPY_APP_TO}"
                    COMMAND ${CMAKE_COMMAND} -E rm -rf "${COPY_APP_TO}/Pitchenga.app"
                    COMMAND cp -R "$<TARGET_BUNDLE_DIR:Pitchenga_Standalone>" "${COPY_APP_TO}/"
                    COMMAND ${CMAKE_COMMAND} -E echo "Copied Pitchenga.app to ${COPY_APP_TO}"
                    VERBATIM)
            endif()
        endif()
    endif()

    if (TARGET Pitchenga_AU)
        target_sources(Pitchenga_AU PRIVATE Source/DesktopAudioCapture.mm)
        target_compile_definitions(Pitchenga_AU PRIVATE PITCHENGA_USE_SCREEN_CAPTURE=0)

        if (NOT DEFINED ENV{CI})
            add_custom_command(TARGET Pitchenga_AU POST_BUILD
                COMMAND /usr/bin/codesign --force --deep --sign "Apple Development" --preserve-metadata=identifier "$<TARGET_BUNDLE_DIR:Pitchenga_AU>"
                VERBATIM)
        endif()
    endif()

    if (TARGET Pitchenga_VST3)
        target_sources(Pitchenga_VST3 PRIVATE Source/DesktopAudioCapture.mm)
        target_compile_definitions(Pitchenga_VST3 PRIVATE PITCHENGA_USE_SCREEN_CAPTURE=0)

        if (NOT DEFINED ENV{CI})
            add_custom_command(TARGET Pitchenga_VST3 POST_BUILD
                COMMAND /usr/bin/codesign --force --deep --sign "Apple Development" --preserve-metadata=identifier "$<TARGET_BUNDLE_DIR:Pitchenga_VST3>"
                VERBATIM)
        endif()
    endif()
else()
    target_sources(Pitchenga PRIVATE Source/DesktopAudioCapture.cpp)
    target_compile_definitions(Pitchenga PRIVATE PITCHENGA_USE_SCREEN_CAPTURE=0)
endif()

target_compile_features(Pitchenga PUBLIC cxx_std_20)

target_compile_definitions(Pitchenga PRIVATE
    CMAKE_BUILD_PROFILE=$<CONFIG>
    JUCE_VST3_CAN_REPLACE_VST2=0)

# Make generated timestamp header available to the build
target_include_directories(Pitchenga PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

juce_add_binary_data(PitchengaData
        SOURCES
        Source/factory-settings.xml)

target_link_libraries(Pitchenga PRIVATE PitchengaData)
