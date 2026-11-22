# AETHERIA — Windows Build Guide

## Prerequisites
- Visual Studio 2026 Community
- Desktop development with C++
- MSVC x64/x86 build tools
- Windows 11 SDK
- C++ CMake tools for Windows
- CMake 3.22+
- Git
- JUCE 9

Current JUCE source directory:
D:\AETHERIA\Dependencies\JUCE

## 1. Clone the development branch

Open PowerShell:

    git clone --branch phase-1-plugin-foundation https://github.com/spvzssg/Aetheria.git D:\AETHERIA\SourceRepo

If that destination already exists, do not delete it. Choose a new empty folder or inspect the existing folder first.

## 2. Configure

    cmake -S D:\AETHERIA\SourceRepo -B D:\AETHERIA\Builds\VS2026 -G "Visual Studio 18 2026" -A x64 -DJUCE_DIR="D:/AETHERIA/Dependencies/JUCE"

If CMake reports that the Visual Studio generator is unknown, run cmake --help and use the exact Visual Studio 2026 generator name listed by your installed CMake.

## 3. Build

    cmake --build D:\AETHERIA\Builds\VS2026 --config Release --parallel 4

Use four parallel jobs initially to keep memory and CPU usage predictable.

## 4. Find the VST3

Expected location:

    D:\AETHERIA\Builds\VS2026\Aetheria_artefacts\Release\VST3\

The exact bundle name and path are determined by JUCE and the build configuration. Verify it exists before installing.

## 5. Test in FL Studio 25

- Open FL Studio 25.
- Open Options → Manage plugins.
- Run a plugin scan.
- Find AETHERIA and load it as an instrument.
- Test MIDI note-on/note-off, velocity, layer levels, master level, attack, release, and state recall.
- Listen for silence, clipping, stuck notes, clicks, or crashes.

## Scope

This is the first playable foundation, not a feature-complete synthesizer. The first build proves the JUCE 9 VST3 target compiles and loads and that the four-layer oscillator voice produces audio.

Do not describe the build as verified until the local compile succeeds and FL Studio loads the resulting plugin.

## Licensing

JUCE is dual-licensed under AGPLv3 and the commercial JUCE licence. Confirm the licence path appropriate to a proprietary commercial product before distributing binaries.
