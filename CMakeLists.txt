cmake_minimum_required(VERSION 3.15)

project(Aetheria VERSION 0.1.0)

# C++20 standard required
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find JUCE
if(NOT DEFINED JUCE_DIR)
    # Try to find JUCE in External subdirectory
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/External/JUCE/CMakeLists.txt")
        set(JUCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/External/JUCE")
        message(STATUS "Found JUCE in External/JUCE")
    else()
        message(FATAL_ERROR 
            "JUCE not found. Please either:\n"
            "  1. Clone JUCE to External/JUCE, or\n"
            "  2. Specify JUCE_DIR: cmake -DJUCE_DIR=<path-to-juce> ..\n"
            "Example: cmake -DJUCE_DIR=C:/JUCE ..")
    endif()
endif()

# Add JUCE to the build
add_subdirectory(${JUCE_DIR} JUCE)

# Plugin formats to build
set(PLUGIN_FORMATS VST3)

# Plugin configuration
juce_add_plugin(Aetheria
    COMPANY_NAME "Spaz SsG"
    PLUGIN_MANUFACTURER_CODE SpSG
    PLUGIN_CODE Aeth
    FORMATS ${PLUGIN_FORMATS}
    PRODUCT_NAME "Aetheria"
    BUNDLE_ID com.spazSsG.aetheria
    
    # Plugin characteristics
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    
    # VST3 specific
    VST3_CATEGORIES "Instrument" "Synth"
    
    # Copy plugin after build
    COPY_PLUGIN_AFTER_BUILD FALSE
)

# Source files - will be added incrementally
target_sources(Aetheria PRIVATE
    Source/Plugin/PluginProcessor.cpp
    Source/Plugin/PluginProcessor.h
    Source/Plugin/PluginEditor.cpp
    Source/Plugin/PluginEditor.h
)

# Include directories
target_include_directories(Aetheria PRIVATE
    Source
    Source/Plugin
    Source/Audio
    Source/Synthesis
    Source/DSP
    Source/Generation
    Source/Library
    Source/Modulation
    Source/Presets
    Source/UI
    Source/Utilities
)

# Compile definitions
target_compile_definitions(Aetheria PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
)

# Link JUCE modules
target_link_libraries(Aetheria PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_plugin_client
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_dsp
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
)

# MSVC-specific settings
if(MSVC)
    target_compile_options(Aetheria PRIVATE
        /W4                     # Warning level 4
        /WX-                    # Don't treat warnings as errors (yet)
        /permissive-           # Standards conformance
        /Zc:__cplusplus        # Correct __cplusplus macro
        /MP                     # Multi-processor compilation
    )
    
    # Release optimizations
    target_compile_options(Aetheria PRIVATE
        $<$<CONFIG:Release>:/O2 /Ob2 /Oi /Ot /GL>
    )
    target_link_options(Aetheria PRIVATE
        $<$<CONFIG:Release>:/LTCG>
    )
endif()

# Installation
install(TARGETS Aetheria
    LIBRARY DESTINATION "C:/Program Files/Common Files/VST3"
    RUNTIME DESTINATION "C:/Program Files/Common Files/VST3"
)

# Print build configuration
message(STATUS "")
message(STATUS "=== Aetheria Configuration ===")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Plugin formats: ${PLUGIN_FORMATS}")
message(STATUS "JUCE directory: ${JUCE_DIR}")
message(STATUS "C++ standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "==============================")
message(STATUS "")
