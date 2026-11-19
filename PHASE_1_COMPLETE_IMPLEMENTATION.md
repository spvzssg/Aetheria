# AETHERIA PHASE 1: COMPLETE IMPLEMENTATION
**Date**: 2026-11-19T04:40:46Z  
**Status**: Ready to Build

This document contains ALL source code for Phase 1. Copy each section to the specified file path.

---

## IMPLEMENTATION CHECKLIST

- [ ] Copy all source files below to specified paths
- [ ] Update CMakeLists.txt (see Section 11)
- [ ] Build: `.\configure.ps1 -Build`
- [ ] Test: Load plugin in FL Studio 25
- [ ] Verify: Play MIDI notes and hear audio

---

## CRITICAL: ONE GLOBAL 32-VOICE POOL

This implementation uses **ONE** VoiceManager with 32 voices total (NOT per-layer).
Each MIDI note can activate up to 4 layer contributions.

---

## SECTION 1: Source/Utilities/Parameters.h

```cpp
#pragma once
#include <JuceHeader.h>

namespace Aetheria {

namespace ParamIDs {
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
    
    // Eight Macros (Phase 1 foundation, Phase 7 full mapping)
    inline constexpr const char* macro_timbre = "macro_timbre";
    inline constexpr const char* macro_motion = "macro_motion";
    inline constexpr const char* macro_space = "macro_space";
    inline constexpr const char* macro_texture = "macro_texture";
    inline constexpr const char* macro_brightness = "macro_brightness";
    inline constexpr const char* macro_warmth = "macro_warmth";
    inline constexpr const char* macro_grit = "macro_grit";
    inline constexpr const char* macro_complexity = "macro_complexity";
}

namespace Constants {
    inline constexpr int maxVoices = 32;
    inline constexpr int numLayers = 4;
    inline constexpr float pitchBendSemitones = 2.0f;
}

enum class LayerRole { SubBody = 0, MidHarmony = 1, AttackTransient = 2, AirTexture = 3 };

} // namespace Aetheria
```

---

## SECTION 2: Source/DSP/Envelope.h

```cpp
#pragma once
#include <JuceHeader.h>

namespace Aetheria {

class Envelope {
public:
    Envelope() = default;
    
    void setSampleRate(double sr) { sampleRate = sr; }
    
    void setParameters(float a, float d, float s, float r) {
        attack = juce::jmax(0.001f, a);
        decay = juce::jmax(0.001f, d);
        sustain = juce::jlimit(0.0f, 1.0f, s);
        release = juce::jmax(0.001f, r);
    }
    
    void noteOn() { state = Attack; counter = 0; }
    
    void noteOff() { 
        if (state != Idle) { 
            state = Release; 
            counter = 0; 
            releaseLevel = level; 
        } 
    }
    
    void reset() { state = Idle; level = 0.0f; counter = 0; }
    
    bool isActive() const { return state != Idle; }
    bool isReleasing() const { return state == Release; }
    
    float getNextSample() {
        if (state == Idle) return 0.0f;
        
        ++counter;
        
        switch (state) {
            case Attack: {
                int samples = static_cast<int>(attack * sampleRate);
                if (counter >= samples) {
                    level = 1.0f;
                    state = Decay;
                    counter = 0;
                } else {
                    level = static_cast<float>(counter) / samples;
                }
                break;
            }
            case Decay: {
                int samples = static_cast<int>(decay * sampleRate);
                if (counter >= samples) {
                    level = sustain;
                    state = Sustain;
                    counter = 0;
                } else {
                    float t = static_cast<float>(counter) / samples;
                    level = 1.0f + t * (sustain - 1.0f);
                }
                break;
            }
            case Sustain:
                level = sustain;
                break;
            case Release: {
                int samples = static_cast<int>(release * sampleRate);
                if (counter >= samples) {
                    level = 0.0f;
                    state = Idle;
                } else {
                    float t = static_cast<float>(counter) / samples;
                    level = releaseLevel * (1.0f - t);
                }
                break;
            }
        }
        return level;
    }
    
private:
    enum State { Idle, Attack, Decay, Sustain, Release };
    State state = Idle;
    double sampleRate = 44100.0;
    float level = 0.0f;
    float releaseLevel = 0.0f;
    float attack = 0.01f, decay = 0.1f, sustain = 0.7f, release = 0.3f;
    int counter = 0;
};

} // namespace Aetheria
```

---

## SECTION 3: Source/DSP/Oscillator.h

```cpp
#pragma once
#include <JuceHeader.h>

namespace Aetheria {

class Oscillator {
public:
    void setSampleRate(double sr) { 
        sampleRate = sr; 
        updateIncrement();
    }
    
    void setFrequency(float freq) { 
        frequency = freq; 
        updateIncrement();
    }
    
    void reset() { phase = 0.0f; }
    
    float getNextSample() {
        float sample = std::sin(phase * juce::MathConstants<float>::twoPi);
        phase += phaseIncrement;
        while (phase >= 1.0f) phase -= 1.0f;
        return sample;
    }
    
private:
    void updateIncrement() {
        phaseIncrement = frequency / static_cast<float>(sampleRate);
    }
    
    double sampleRate = 44100.0;
    float frequency = 440.0f;
    float phase = 0.0f;
    float phaseIncrement = 0.0f;
};

} // namespace Aetheria
```

---

## SECTION 4: Source/DSP/Filter.h

```cpp
#pragma once
#include <JuceHeader.h>

namespace Aetheria {

class Filter {
public:
    void setSampleRate(double sr) { 
        sampleRate = sr; 
        updateCoefficients(); 
    }
    
    void setParameters(float cutoff, float res) {
        frequency = juce::jlimit(20.0f, 20000.0f, cutoff);
        resonance = juce::jlimit(0.1f, 10.0f, res);
        updateCoefficients();
    }
    
    void reset() { z1 = z2 = 0.0f; }
    
    float process(float input) {
        float output = a0 * input + a1 * z1 + a2 * z2 - b1 * z1 - b2 * z2;
        z2 = z1;
        z1 = input;
        return output;
    }
    
private:
    void updateCoefficients() {
        if (sampleRate <= 0.0) return;
        
        float w0 = juce::MathConstants<float>::twoPi * frequency / static_cast<float>(sampleRate);
        float cosw0 = std::cos(w0);
        float sinw0 = std::sin(w0);
        float alpha = sinw0 / (2.0f * resonance);
        
        float b0 = 1.0f + alpha;
        a0 = ((1.0f - cosw0) / 2.0f) / b0;
        a1 = (1.0f - cosw0) / b0;
        a2 = a0;
        b1 = (-2.0f * cosw0) / b0;
        b2 = (1.0f - alpha) / b0;
    }
    
    double sampleRate = 44100.0;
    float frequency = 1000.0f;
    float resonance = 0.707f;
    float a0 = 0.0f, a1 = 0.0f, a2 = 0.0f, b1 = 0.0f, b2 = 0.0f;
    float z1 = 0.0f, z2 = 0.0f;
};

} // namespace Aetheria
```

---

**[CONTINUED IN NEXT MESSAGE - Voice, VoiceManager, and PluginProcessor require more space]**

This is Part 1 of the Phase 1 implementation. The remaining critical files (Voice, VoiceManager, PluginProcessor updates) will follow in the next message to stay within token limits.

