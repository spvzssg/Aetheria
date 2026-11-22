# AETHERIA by Spaz SsG

**Infinite Sound. Unlimited Imagination.**

AETHERIA is a Windows VST3 generative hybrid instrument in development, built with C++20, JUCE 9, and CMake for FL Studio 25.

## Current development status

**Phase 1 foundation in progress.** This branch contains the first playable plugin foundation:
- JUCE VST3 instrument target
- 32-voice polyphonic synthesiser
- Four oscillator layers: A Sub/Body, B Mid/Harmony, C Attack, D Air/Texture
- MIDI note input and velocity response
- Attack and release envelope controls
- Master and per-layer level controls
- JUCE parameter state serialization
- Initial dark AETHERIA editor

This is an early playable foundation, not the finished AETHERIA instrument. Advanced synthesis engines, sample playback, generative prompt system, deterministic randomization, mutation, Sound DNA, effects, modulation, and the final reference-matched GUI remain future milestones.

## Build requirements

- Windows 11 x64
- Visual Studio 2026 Community with Desktop development with C++
- CMake 3.22 or newer
- Git
- JUCE 9 source tree
- Commercial JUCE licence for proprietary commercial distribution, or a deliberate decision to comply with AGPLv3

## Workspace layout

Recommended local layout:

    D:\AETHERIA\
      Dependencies\JUCE\       # Existing JUCE source
      SourceRepo\               # Clone of this repository
      Builds\VS2026\           # Generated build files

## Configure and build (PowerShell)

Clone the development branch:

    git clone --branch phase-1-plugin-foundation https://github.com/spvzssg/Aetheria.git D:\AETHERIA\SourceRepo

Configure with Visual Studio 2026:

    cmake -S D:\AETHERIA\SourceRepo -B D:\AETHERIA\Builds\VS2026 -G "Visual Studio 18 2026" -A x64 -DJUCE_DIR="D:/AETHERIA/Dependencies/JUCE"

Build Release:

    cmake --build D:\AETHERIA\Builds\VS2026 --config Release --parallel 4

The expected VST3 bundle is under:

    D:\AETHERIA\Builds\VS2026\Aetheria_artefacts\Release\VST3\

Confirm the actual generated bundle path in the build output before copying it. Do not install over an existing plugin without checking its version first.

## FL Studio 25 test

1. Confirm the .vst3 bundle exists.
2. Copy the bundle to the user VST3 folder or system VST3 folder only after the build succeeds.
3. In FL Studio, open Options → Manage plugins and run a scan.
4. Load AETHERIA as an instrument.
5. Verify the editor opens and MIDI notes produce sound.
6. Verify layer levels, master, attack, release, and project state recall.

## Audio architecture

The initial voice combines four independently adjustable oscillator layers:
- **A — Sub / Body:** octave-down foundation
- **B — Mid / Harmony:** fundamental
- **C — Attack:** upper harmonic emphasis
- **D — Air / Texture:** high partial layer

The initial engine is intentionally small and CPU-conscious. It is a starting point for later engine abstraction, wavetable/FM/sample/granular sources, modulation, and effects.

## Licensing

JUCE modules are dual-licensed under AGPLv3 and the commercial JUCE licence. A proprietary commercial release must use an appropriate commercial JUCE licence or otherwise comply with the applicable open-source licence obligations. Review JUCE's current terms before distribution:
- https://juce.com/legal/juce-9-licence/
- https://www.gnu.org/licenses/agpl-3.0.en.html

AETHERIA source code is proprietary to Spaz SsG unless a file explicitly states otherwise. VST3 is a Steinberg trademark.

## Development branch

The active foundation branch is phase-1-plugin-foundation. Keep development and build verification on this branch until the milestone is tested; do not merge into main until the VST3 builds and passes host smoke tests.
