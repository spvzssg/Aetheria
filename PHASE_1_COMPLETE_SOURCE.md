# PHASE 1: PLUGIN FOUNDATION - COMPLETE IMPLEMENTATION
**Project**: Aetheria by Spaz SsG  
**Phase**: 1 - Plugin Foundation  
**Status**: Ready for Implementation  
**Date**: 2026-11-19T04:37:00Z

---

## IMPLEMENTATION CHECKLIST

Before implementing Phase 1:
- [ ] Phase 0 directories created via `setup_directories.bat`
- [ ] JUCE 7.0.12 installed in `External\JUCE`
- [ ] Phase 0 builds successfully

After implementing Phase 1:
- [ ] All source files below created
- [ ] CMakeLists.txt updated (see Step 8)
- [ ] Project builds without errors
- [ ] Plugin loads and produces MIDI-driven audio
- [ ] All tests pass

---

## CRITICAL ARCHITECTURE NOTES

**ONE GLOBAL 32-VOICE ALLOCATOR**:
- NOT four separate 32-voice allocators
- NOT 32 voices per layer
- ONE pool of 32 voices total
- Each active MIDI note can use up to 4 layers
- Maximum: 32 MIDI notes × 4 layers = 128 layer contributions

**Real-Time Safety**:
- No allocations in processBlock()
- No disk I/O in audio thread
- Pre-allocated voice pool
- Lock-free parameter reading

---

## FILE STRUCTURE

```
Source/
├── Utilities/
│   ├── Parameters.h              [NEW - Step 1]
│   └── Constants.h                [NEW - Step 2]
├── DSP/
│   ├── Envelope.h                 [NEW - Step 3]
│   ├── Envelope.cpp               [NEW - Step 3]
│   ├── Oscillator.h               [NEW - Step 4]
│   ├── Oscillator.cpp             [NEW - Step 4]
│   ├── Filter.h                   [NEW - Step 5]
│   └── Filter.cpp                 [NEW - Step 5]
├── Audio/
│   ├── Voice.h                    [NEW - Step 6]
│   ├── Voice.cpp                  [NEW - Step 6]
│   ├── VoiceManager.h             [NEW - Step 7]
│   └── VoiceManager.cpp           [NEW - Step 7]
└── Plugin/
    ├── PluginProcessor.h          [REPLACE - Step 9]
    ├── PluginProcessor.cpp        [REPLACE - Step 9]
    ├── PluginEditor.h             [UPDATE - Step 10]
    └── PluginEditor.cpp           [UPDATE - Step 10]
```

---

## STEP 1: Parameters.h

**File**: `Source/Utilities/Parameters.h`

```cpp
#pragma once

#include <JuceHeader.h>

namespace Aetheria
{

// Parameter ID constants (stable - never change these)
namespace ParamIDs
{
    // Master
    inline constexpr const char* masterVolume = "masterVolume";
    
    // Layer A (Sub/Body)
    inline constexpr const char* layerA_enabled = "layerA_enabled";
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
    
    // Layer B (Mid/Harmony)
    inline constexpr const char* layerB_enabled = "layerB_enabled";
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
    
    // Layer C (Attack/Transient)
    inline constexpr const char* layerC_enabled = "layerC_enabled";
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
    
    // Layer D (Air/Texture)
    inline constexpr const char* layerD_enabled = "layerD_enabled";
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
    
    // Eight Vibe/Macro controls (Phase 1 foundation)
    inline constexpr const char* macro_timbre = "macro_timbre";
    inline constexpr const char* macro_motion = "macro_motion";
    inline constexpr const char* macro_space = "macro_space";
    inline constexpr const char* macro_texture = "macro_texture";
    inline constexpr const char* macro_brightness = "macro_brightness";
    inline constexpr const char* macro_warmth = "macro_warmth";
    inline constexpr const char* macro_grit = "macro_grit";
    inline constexpr const char* macro_complexity = "macro_complexity";
}

// Audio constants
namespace Constants
{
    inline constexpr int maxVoices = 32;        // Global pool
    inline constexpr int numLayers = 4;         // A, B, C, D
    inline constexpr float pitchBendSemitones = 2.0f;
    inline constexpr float defaultSampleRate = 44100.0f;
    inline constexpr float filterMinFreq = 20.0f;
    inline constexpr float filterMaxFreq = 20000.0f;
}

// Layer roles
enum class LayerRole
{
    SubBody = 0,        // Layer A
    MidHarmony = 1,     // Layer B
    AttackTransient = 2,// Layer C
    AirTexture = 3      // Layer D
};

} // namespace Aetheria
```

---

## STEP 2: Envelope.h and Envelope.cpp

**File**: `Source/DSP/Envelope.h`

```cpp
#pragma once

#include <JuceHeader.h>

namespace Aetheria
{

class Envelope
{
public:
    Envelope();
    
    void setSampleRate(double sampleRate);
    void setParameters(float attackSec, float decaySec, float sustainLevel, float releaseSec);
    
    void noteOn();
    void noteOff();
    void reset();
    
    float getNextSample();
    bool isActive() const { return currentState != State::Idle; }
    bool isReleasing() const { return currentState == State::Release; }
    
private:
    enum class State { Idle, Attack, Decay, Sustain, Release };
    
    State currentState = State::Idle;
    double sampleRate = 44100.0;
    float currentLevel = 0.0f;
    
    float attackTime = 0.01f;
    float decayTime = 0.1f;
    float sustainLevel = 0.7f;
    float releaseTime = 0.3f;
    
    int samplesSinceStateChange = 0;
};

} // namespace Aetheria
```

**File**: `Source/DSP/Envelope.cpp`

```cpp
#include "Envelope.h"

namespace Aetheria
{

Envelope::Envelope()
{
}

void Envelope::setSampleRate(double sr)
{
    sampleRate = sr;
}

void Envelope::setParameters(float attackSec, float decaySec, float sustainLvl, float releaseSec)
{
    attackTime = juce::jmax(0.001f, attackSec);
    decayTime = juce::jmax(0.001f, decaySec);
    sustainLevel = juce::jlimit(0.0f, 1.0f, sustainLvl);
    releaseTime = juce::jmax(0.001f, releaseSec);
}

void Envelope::noteOn()
{
    currentState = State::Attack;
    samplesSinceStateChange = 0;
}

void Envelope::noteOff()
{
    if (currentState != State::Idle)
    {
        currentState = State::Release;
        samplesSinceStateChange = 0;
    }
}

void Envelope::reset()
{
    currentState = State::Idle;
    currentLevel = 0.0f;
    samplesSinceStateChange = 0;
}

float Envelope::getNextSample()
{
    if (currentState == State::Idle)
        return 0.0f;
    
    ++samplesSinceStateChange;
    
    switch (currentState)
    {
        case State::Attack:
        {
            int attackSamples = static_cast<int>(attackTime * sampleRate);
            if (samplesSinceStateChange >= attackSamples)
            {
                currentLevel = 1.0f;
                currentState = State::Decay;
                samplesSinceStateChange = 0;
            }
            else
            {
                currentLevel = static_cast<float>(samplesSinceStateChange) / attackSamples;
            }
            break;
        }
        
        case State::Decay:
        {
            int decaySamples = static_cast<int>(decayTime * sampleRate);
            if (samplesSinceStateChange >= decaySamples)
            {
                currentLevel = sustainLevel;
                currentState = State::Sustain;
                samplesSinceStateChange = 0;
            }
            else
            {
                float t = static_cast<float>(samplesSinceStateChange) / decaySamples;
                currentLevel = 1.0f + t * (sustainLevel - 1.0f);
            }
            break;
        }
        
        case State::Sustain:
            currentLevel = sustainLevel;
            break;
        
        case State::Release:
        {
            int releaseSamples = static_cast<int>(releaseTime * sampleRate);
            if (samplesSinceStateChange >= releaseSamples)
            {
                currentLevel = 0.0f;
                currentState = State::Idle;
            }
            else
            {
                float startLevel = sustainLevel;
                if (samplesSinceStateChange == 0)
                    startLevel = currentLevel;
                
                float t = static_cast<float>(samplesSinceStateChange) / releaseSamples;
                currentLevel = startLevel * (1.0f - t);
            }
            break;
        }
        
        default:
            break;
    }
    
    return currentLevel;
}

} // namespace Aetheria
```

---

This implementation guide is becoming very long. Let me create a more practical approach - I'll create a downloadable ZIP-style archive document with all complete source files that the user can directly copy and paste.

Let me create a comprehensive but more manageable implementation document:

