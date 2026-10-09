cmake_minimum_required(VERSION 3.22)

project(SJChannelStrip VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------------------------------------------------------------
# macOS Platform Configuration (Apple Silicon arm64 first, Intel optional)
# ------------------------------------------------------------------------------
if (APPLE)
    if (NOT CMAKE_OSX_ARCHITECTURES)
        # Target Apple Silicon arm64 first as default; can be overridden via -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
        set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "macOS target architectures" FORCE)
    endif()

    if (NOT CMAKE_OSX_DEPLOYMENT_TARGET)
        # macOS 11.0 Big Sur minimum required for Apple Silicon arm64 support
        set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS deployment target" FORCE)
    endif()

    message(STATUS "macOS Target Architecture: ${CMAKE_OSX_ARCHITECTURES}")
    message(STATUS "macOS Deployment Target:   ${CMAKE_OSX_DEPLOYMENT_TARGET}")
endif()

# ------------------------------------------------------------------------------
# Locate JUCE
# ------------------------------------------------------------------------------
if (NOT TARGET juce::juce_core)
    if (DEFINED JUCE_PATH AND EXISTS "${JUCE_PATH}")
        message(STATUS "Using JUCE from JUCE_PATH: ${JUCE_PATH}")
        add_subdirectory("${JUCE_PATH}" juce_build EXCLUDE_FROM_ALL)
    elseif (EXISTS "C:/Users/bhoom/Downloads/juce-9.0.3-windows/JUCE")
        message(STATUS "Using local Windows JUCE 9.0.3 installation")
        add_subdirectory("C:/Users/bhoom/Downloads/juce-9.0.3-windows/JUCE" juce_build EXCLUDE_FROM_ALL)
    elseif (APPLE AND EXISTS "$ENV{HOME}/JUCE")
        message(STATUS "Using JUCE from user home directory: $ENV{HOME}/JUCE")
        add_subdirectory("$ENV{HOME}/JUCE" juce_build EXCLUDE_FROM_ALL)
    elseif (APPLE AND EXISTS "/Applications/JUCE")
        message(STATUS "Using JUCE from /Applications/JUCE")
        add_subdirectory("/Applications/JUCE" juce_build EXCLUDE_FROM_ALL)
    else()
        find_package(JUCE CONFIG QUIET)
        if (NOT TARGET juce::juce_core)
            message(STATUS "Fetching JUCE via FetchContent...")
            include(FetchContent)
            set(SJ_JUCE_TAG "8.0.6" CACHE STRING "JUCE Git tag to fetch if local JUCE is not found")
            FetchContent_Declare(
                JUCE
                GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
                GIT_TAG ${SJ_JUCE_TAG}
            )
            FetchContent_MakeAvailable(JUCE)
        endif()
    endif()
endif()

# ------------------------------------------------------------------------------
# Plugin Formats Configuration
# ------------------------------------------------------------------------------
# VST3 is required for FL Studio (both Windows and macOS). Standalone is always supported.
set(SJ_PLUGIN_FORMATS VST3 Standalone)

# AudioUnit (AU / Component) is native to macOS (Logic Pro, GarageBand, Ableton Live, Reaper, etc.)
if (APPLE)
    option(SJ_ENABLE_AU "Enable AudioUnit (AU) format on macOS" ON)
    if (SJ_ENABLE_AU)
        list(APPEND SJ_PLUGIN_FORMATS AU)
    endif()
endif()

message(STATUS "Configured Plugin Formats: ${SJ_PLUGIN_FORMATS}")

# ------------------------------------------------------------------------------
# Plugin Target Definition
# ------------------------------------------------------------------------------
juce_add_plugin(SJChannelStrip
    COMPANY_NAME "SJ Audio"
    BUNDLE_ID "com.sjaudio.sjchannelstrip"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    VST3_AUTO_MANIFEST FALSE
    PLUGIN_MANUFACTURER_CODE "Sjau"
    PLUGIN_CODE "Sjcs"
    FORMATS ${SJ_PLUGIN_FORMATS}
    PRODUCT_NAME "SJ Channel Strip"
    DESCRIPTION "Professional Analog-Inspired Console Channel Strip"
)

# ------------------------------------------------------------------------------
# Source Files
# ------------------------------------------------------------------------------
target_sources(SJChannelStrip PRIVATE
    Source/Parameters.h
    Source/PluginProcessor.h
    Source/PluginProcessor.cpp
    Source/PluginEditor.h
    Source/PluginEditor.cpp
    Source/DSP/InputStage.h
    Source/DSP/InputStage.cpp
    Source/DSP/OutputStage.h
    Source/DSP/OutputStage.cpp
    Source/DSP/Metering.h
    Source/DSP/Metering.cpp
    Source/DSP/SpectrumFIFO.h
    Source/DSP/FilterSection.h
    Source/DSP/FilterSection.cpp
    Source/DSP/EQBand.h
    Source/DSP/EQBand.cpp
    Source/DSP/EQSection.h
    Source/DSP/EQSection.cpp
    Source/DSP/GateExpander.h
    Source/DSP/GateExpander.cpp
    Source/DSP/Compressor.h
    Source/DSP/Compressor.cpp
    Source/DSP/Saturation.h
    Source/DSP/Saturation.cpp
    Source/DSP/DynamicEQ.h
    Source/DSP/DynamicEQ.cpp
    Source/PresetManager.h
    Source/PresetManager.cpp
    Source/GUI/SJLookAndFeel.h
    Source/GUI/SJLookAndFeel.cpp
    Source/GUI/SJComponents.h
    Source/GUI/SJComponents.cpp
    Source/GUI/Meter.h
    Source/GUI/Meter.cpp
    Source/GUI/FilterDisplay.h
    Source/GUI/FilterDisplay.cpp
    Source/GUI/EQDisplay.h
    Source/GUI/EQDisplay.cpp
    Source/GUI/GateGRMeter.h
    Source/GUI/GateGRMeter.cpp
    Source/GUI/CompressorGRMeter.h
    Source/GUI/CompressorGRMeter.cpp
    Source/GUI/SaturationMeter.h
    Source/GUI/SaturationMeter.cpp
    Source/GUI/DynamicEQMeter.h
    Source/GUI/DynamicEQMeter.cpp
)

# ------------------------------------------------------------------------------
# Include Directories & Compiler Options
# ------------------------------------------------------------------------------
target_include_directories(SJChannelStrip PRIVATE
    Source
)

target_compile_definitions(SJChannelStrip PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

if (MSVC)
    target_compile_options(SJChannelStrip PRIVATE /fp:fast)
elseif (APPLE)
    target_compile_options(SJChannelStrip PRIVATE -Wall -Wextra -Wno-unused-parameter)
endif()

# ------------------------------------------------------------------------------
# JUCE Module Linkage
# ------------------------------------------------------------------------------
target_link_libraries(SJChannelStrip PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

# ------------------------------------------------------------------------------
# Automated Audit Test Suite Target (Optional)
# ------------------------------------------------------------------------------
option(SJ_BUILD_TESTS "Build automated audit test suite" OFF)
if (SJ_BUILD_TESTS)
    add_executable(run_audit_tests tests/run_audit_tests.cpp)
    target_include_directories(run_audit_tests PRIVATE Source)
    target_link_libraries(run_audit_tests PRIVATE
        SJChannelStrip
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
    )
endif()

