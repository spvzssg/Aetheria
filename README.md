# AETHERIA by Spaz SsG

**Infinite Sound. Unlimited Imagination.**

AETHERIA is a Windows VST3 generative hybrid instrument in development, built with C++20, JUCE 9, and CMake for FL Studio 25.

## Development status

**Phase 2 — independent layer foundation.** This branch replaces the original four fixed-ratio sine oscillator mix with four separately selectable sound layers:

- **A — Sub / Body:** sine sub, triangle, saw, square, deep sub.
- **B — Mid / Harmony:** warm saw, detuned saw, pulse, FM keys, sine.
- **C — Attack / Transient:** pluck, FM bell, mallet, noise hit, click.
- **D — Air / Texture:** white noise, pink-noise approximation, air/noise, sine cloud, crackle.
- Independent amplitude envelopes (attack, decay, sustain, release) for each layer, with separate parameter state.
- Per-layer source selection and level controls in the editor.
- Existing 32-voice MIDI architecture, master level, global envelope, and JUCE state serialization retained.

The current Phase 2 sources are lightweight first-pass algorithms implemented in AETHERIA's own voice renderer. They are **not yet** the DaisySP, Q, or DSPark engines, and the D-layer noise/air sources are not a full sample-based granular engine. These integrations are planned after API, build, and real-time-safety validation.

## Planned engine integrations and research references

- **DaisySP:** candidate for physical modeling, plucks, noise, granular player and synthesis modules. MIT-licensed; retain upstream notices.
- **Cycfi Q:** candidate/reference for virtual analog, FM, resonant filters, envelopes and granular DSP. MIT-licensed; retain upstream notices.
- **DSPark:** candidate for synthesizer, wavetable, sample, modulation, effects and analysis infrastructure. MIT-licensed; validate the exact v1.8.1 APIs and real-time behavior before adoption.
- **TransDes:** candidate for transient shaping. MIT-licensed; retain attribution and inspect its algorithm provenance before incorporating code.
- **Oi-Grandad, HISE, AudibleInstruments and Surge XT:** architectural and sound-design references only at this stage. Their GPL licensing means source code must not be copied into a proprietary distribution without an appropriate licensing decision.

## Build requirements

- Windows 11 x64
- Visual Studio 2026 Community with Desktop development with C++
- CMake 4.2+ for the Visual Studio 18 2026 generator
- Git
- JUCE 9 source tree
- Appropriate JUCE commercial licence for proprietary distribution, or a deliberate decision to comply with AGPLv3

## Workspace layout

    D:\AETHERIA\
      Dependencies\JUCE\
      SourceRepo\
      Builds\VS2026-Phase2\

## Configure and build (PowerShell)

    cmake -S D:\AETHERIA\SourceRepo -B D:\AETHERIA\Builds\VS2026-Phase2 -G "Visual Studio 18 2026" -A x64 -DJUCE_DIR="D:/AETHERIA/Dependencies/JUCE"

    cmake --build D:\AETHERIA\Builds\VS2026-Phase2 --config Release --parallel 4

The VST3 bundle is generated under:

    D:\AETHERIA\Builds\VS2026-Phase2\Aetheria_artefacts\Release\VST3\

Confirm the actual path in build output before installing. Do not overwrite the existing Phase 1 plugin until Phase 2 passes its host tests.

## Audio architecture roadmap

1. Four genuinely independent layer source engines and envelopes.
2. Per-layer filters, pan, octave, pitch, solo/mute/lock and metering.
3. DaisySP and/or DSPark integrations selected by feature and measured performance.
4. Sample playback and full granular engine.
5. Modulation, effects, deterministic seed/randomization, mutation, Sound DNA and presets.
6. Reference-matched AETHERIA GUI and comprehensive host validation.

## Licensing

JUCE modules are dual-licensed under AGPLv3 and the commercial JUCE licence. A proprietary commercial release must use an appropriate commercial JUCE licence or otherwise comply with applicable open-source obligations.

Third-party components must retain their respective copyright and licence notices. MIT does not mean attribution can be omitted. GPL-licensed reference projects are not included in the Phase 2 build.

Review JUCE's current terms:
- https://juce.com/legal/juce-9-licence/
- https://www.gnu.org/licenses/agpl-3.0.en.html

AETHERIA source code is proprietary to Spaz SsG unless a file explicitly states otherwise. VST3 is a Steinberg trademark.

## Development branch

Active branch: `phase-2-independent-layers`. Keep Phase 1 intact until this branch builds and passes FL Studio smoke tests. Do not merge into `main` before verification.
