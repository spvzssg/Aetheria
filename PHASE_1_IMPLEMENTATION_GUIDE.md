# PHASE 1 IMPLEMENTATION GUIDE
**Project**: Aetheria by Spaz SsG  
**Phase**: 1 - Plugin Foundation  
**Date**: 2026-11-19T04:36:30Z

---

## PREREQUISITES

Before starting Phase 1, ensure Phase 0 is complete:
1. ✅ Directory structure created (`setup_directories.bat`)
2. ✅ JUCE 7.0.12 installed in `External\JUCE`
3. ✅ Phase 0 source files created (PluginProcessor.h/cpp, PluginEditor.h/cpp)
4. ✅ Project builds successfully

---

## PHASE 1 OVERVIEW

Phase 1 implements:
- **Global 32-voice allocator** (NOT per-layer)
- **Four-layer architecture** (A: Sub/Body, B: Mid/Harmony, C: Attack/Transient, D: Air/Texture)
- **MIDI handling**: note on/off, velocity, sustain pedal, pitch bend
- **Basic oscillator**: Band-limited sine wave synthesis
- **ADSR envelopes** per layer
- **Basic filters** per layer
- **Parameter system**: AudioProcessorValueTreeState with stable IDs
- **Eight macro foundations** (full implementation in Phase 7)
- **State serialization**: Save/load plugin state
- **Real-time safety**: No allocations in audio callback

---

## IMPLEMENTATION STEPS

### Step 1: Create Utilities Files

**File**: `Source/Utilities/Parameters.h`

```cpp
// Phase 1: Core parameter identifiers and constants
#pragma once

#include <JuceHeader.h>

namespace Aetheria
{

//==============================================================================
// Parameter ID constants (stable - do not change)
//==============================================================================

namespace ParamIDs
{
    // Master controls
    inline constexpr const char* masterVolume = "masterVolume";
    
    // Layer A (Sub/Body) parameters
    inline constexpr const char* layerA_enabled = "layerA_enabled";
    inline constexpr const char* layerA_locked = "layerA_locked";
    inline constexpr const char* layerA_solo = "layerA_solo";
    inline constexpr const char* layerA_mute = "layerA_mute";
    inline constexpr const char* layerA_level = "layerA_level";
    inline constexpr const char* layerA_pan = "layerA_pan";
    inline constexpr const char* layerA_octave = "layerA_octave";
    inline constexpr const char* layerA_pitch = "layerA_pitch";
    inline constexpr const char* layerA_attack = "layerA_attack";
    inline constexpr const char* layerA_decay = "layerA_decay";
    inline constexpr const char* layerA_sustain = "layerA_sustain";
    inline constexpr const char* layerA_release = "layerA_release";
    inline constexpr const char* layerA_filterCutoff = "layerA_filterCutoff";
    inline constexpr const char* layerA_filterResonance = "layerA_filterResonance";
    
    // Layer B (Mid/Harmony) parameters
    inline constexpr const char* layerB_enabled = "layerB_enabled";
    inline constexpr const char* layerB_locked = "layerB_locked";
    inline constexpr const char* layerB_solo = "layerB_solo";
    inline constexpr const char* layerB_mute = "layerB_mute";
    inline constexpr const char* layerB_level = "layerB_level";
    inline constexpr const char* layerB_pan = "layerB_pan";
    inline constexpr const char* layerB_octave = "layerB_octave";
    inline constexpr const char* layerB_pitch = "layerB_pitch";
    inline constexpr const char* layerB_attack = "layerB_attack";
    inline constexpr const char* layerB_decay = "layerB_decay";
    inline constexpr const char* layerB_sustain = "layerB_sustain";
    inline constexpr const char* layerB_release = "layerB_release";
    inline constexpr const char* layerB_filterCutoff = "layerB_filterCutoff";
    inline constexpr const char* layerB_filterResonance = "layerB_filterResonance";
    
    // Layer C (Attack/Transient) parameters
    inline constexpr const char* layerC_enabled = "layerC_enabled";
    inline constexpr const char* layerC_locked = "layerC_locked";
    inline constexpr const char* layerC_solo = "layerC_solo";
    inline constexpr const char* layerC_mute = "layerC_mute";
    inline constexpr const char* layerC_level = "layerC_level";
    inline constexpr const char* layerC_pan = "layerC_pan";
    inline constexpr const char* layerC_octave = "layerC_octave";
    inline constexpr const char* layerC_pitch = "layerC_pitch";
    inline constexpr const char* layerC_attack = "layerC_attack";
    inline constexpr const char* layerC_decay = "layerC_decay";
    inline constexpr const char* layerC_sustain = "layerC_sustain";
    inline constexpr const char* layerC_release = "layerC_release";
    inline constexpr const char* layerC_filterCutoff = "layerC_filterCutoff";
    inline constexpr const char* layerC_filterResonance = "layerC_filterResonance";
    
    // Layer D (Air/Texture) parameters
    inline constexpr const char* layerD_enabled = "layerD_enabled";
    inline constexpr const char* layerD_locked = "layerD_locked";
    inline constexpr const char* layerD_solo = "layerD_solo";
    inline constexpr const char* layerD_mute = "layerD_mute";
    inline constexpr const char* layerD_level = "layerD_level";
    inline constexpr const char* layerD_pan = "layerD_pan";
    inline constexpr const char* layerD_octave = "layerD_octave";
    inline constexpr const char* layerD_pitch = "layerD_pitch";
    inline constexpr const char* layerD_attack = "layerD_attack";
    inline constexpr const char* layerD_decay = "layerD_decay";
    inline constexpr const char* layerD_sustain = "layerD_sustain";
    inline constexpr const char* layerD_release = "layerD_release";
    inline constexpr const char* layerD_filterCutoff = "layerD_filterCutoff";
    inline constexpr const char* layerD_filterResonance = "layerD_filterResonance";
    
    // Eight Vibe/Macro controls (Phase 1 foundation, Phase 7 full implementation)
    inline constexpr const char* macro_timbre = "macro_timbre";
    inline constexpr const char* macro_motion = "macro_motion";
    inline constexpr const char* macro_space = "macro_space";
    inline constexpr const char* macro_texture = "macro_texture";
    inline constexpr const char* macro_brightness = "macro_brightness";
    inline constexpr const char* macro_warmth = "macro_warmth";
    inline constexpr const char* macro_grit = "macro_grit";
    inline constexpr const char* macro_complexity = "macro_complexity";
}

//==============================================================================
// Audio constants
//==============================================================================

namespace Constants
{
    // Voice management
    inline constexpr int maxVoices = 32;
    inline constexpr int numLayers = 4;
    
    // MIDI
    inline constexpr int midiNoteMin = 0;
    inline constexpr int midiNoteMax = 127;
    inline constexpr float pitchBendSemitones = 2.0f;
    
    // Audio
    inline constexpr float minFrequency = 20.0f;
    inline constexpr float maxFrequency = 20000.0f;
    inline constexpr float defaultSampleRate = 44100.0f;
    
    // Filter
    inline constexpr float filterMinFreq = 20.0f;
    inline constexpr float filterMaxFreq = 20000.0f;
    inline constexpr float filterMinQ = 0.1f;
    inline constexpr float filterMaxQ = 10.0f;
}

//==============================================================================
// Layer roles (for UI and organization)
//==============================================================================

enum class LayerRole
{
    SubBody = 0,        // Layer A: Foundation and body
    MidHarmony = 1,     // Layer B: Harmonic content
    AttackTransient = 2,// Layer C: Transient and impact
    AirTexture = 3      // Layer D: Atmospheric texture
};

} // namespace Aetheria
```

---

Due to the length of the complete Phase 1 implementation, I'll create a structured implementation document. This will be extensive (thousands of lines of code), so I'll provide it systematically.

Let me create the complete Phase 1 implementation guide with all source files:

