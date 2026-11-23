cmake_minimum_required(VERSION 3.22)

project(Aetheria VERSION 0.2.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# JUCE can live beside the source checkout in D:/AETHERIA/Dependencies/JUCE,
# or be supplied explicitly with -DJUCE_DIR=...
if(NOT DEFINED JUCE_DIR)
    set(_aetheria_juce_candidates
        "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/JUCE"
        "${CMAKE_CURRENT_SOURCE_DIR}/External/JUCE"
    )
    foreach(_candidate IN LISTS _aetheria_juce_candidates)
        if(EXISTS "${_candidate}/CMakeLists.txt")
            set(JUCE_DIR "${_candidate}")
            break()
        endif()
    endforeach()
endif()

if(NOT DEFINED JUCE_DIR OR NOT EXISTS "${JUCE_DIR}/CMakeLists.txt")
    message(FATAL_ERROR
        "JUCE was not found. Pass -DJUCE_DIR=<path-to-JUCE> when configuring.")
endif()

add_subdirectory("${JUCE_DIR}" "${CMAKE_BINARY_DIR}/JUCE")

juce_add_plugin(Aetheria
    COMPANY_NAME "Spaz SsG"
    PLUGIN_MANUFACTURER_CODE SpSG
    PLUGIN_CODE AeP3
    PRODUCT_NAME "AETHERIA Phase 3"
    BUNDLE_ID com.spazssg.aetheria.phase3
    VERSION "${PROJECT_VERSION}"
    FORMATS VST3
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    VST3_CATEGORIES Instrument Synth
)

juce_generate_juce_header(Aetheria)

target_sources(Aetheria PRIVATE
    Source/Plugin/PluginProcessor.cpp
    Source/Plugin/PluginProcessor.h
    Source/Plugin/PluginEditor.cpp
    Source/Plugin/PluginEditor.h
)

target_compile_definitions(Aetheria PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
)

target_link_libraries(Aetheria PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

if(MSVC)
    target_compile_options(Aetheria PRIVATE /W4 /permissive- /Zc:__cplusplus /MP)
endif()

message(STATUS "AETHERIA ${PROJECT_VERSION}")
message(STATUS "JUCE_DIR: ${JUCE_DIR}")
message(STATUS "Plugin formats: VST3")
message(STATUS "C++ standard: C++${CMAKE_CXX_STANDARD}")
