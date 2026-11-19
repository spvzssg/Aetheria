# Aetheria Technical Architecture

## Overview
Aetheria is a professional generative hybrid synthesizer built as a Windows VST3 plugin using C++20, JUCE framework, and modern audio DSP practices.

## Core Principles
1. **Real-time safety**: No allocations, disk I/O, or blocking operations in audio callback
2. **Modular design**: Clear separation between audio processing, UI, and business logic
3. **Deterministic generation**: Same seed + settings = same patch
4. **Professional quality**: 32-voice polyphony, pristine audio, stable performance

## System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    VST3 Plugin Host                          │
│                     (JUCE Framework)                          │
└─────────────────────────────────────────────────────────────┘
                              │
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                    AudioProcessor                            │
│  - Plugin lifecycle management                               │
│  - Parameter management (APVTS)                              │
│  - State serialization                                       │
│  - Host communication                                        │
└─────────────────────────────────────────────────────────────┘
                              │
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                    Audio Engine                              │
│  ┌──────────────────────────────────────────────────────┐  │
│  │           Global Voice Manager                        │  │
│  │  - 32 simultaneous MIDI voices                       │  │
│  │  - Voice allocation & stealing                       │  │
│  │  - MIDI event handling                               │  │
│  │  - Note lifecycle management                         │  │
│  └──────────────────────────────────────────────────────┘  │
│                              │                               │
│                              ↓                               │
│  ┌──────────────────────────────────────────────────────┐  │
│  │           Layer Processing (A, B, C, D)              │  │
│  │  Each layer contains:                                │  │
│  │  - Synthesis engine instance                         │  │
│  │  - ADSR envelope                                     │  │
│  │  - Filter (type, cutoff, resonance)                 │  │
│  │  - Per-layer effects chain                          │  │
│  │  - Enable/lock/solo/mute state                      │  │
│  └──────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
                              │
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                 Synthesis Engines                            │
│  ┌─────────────┬─────────────┬─────────────┬────────────┐  │
│  │   Sample    │  Wavetable  │ Subtractive │     FM     │  │
│  ├─────────────┼─────────────┼─────────────┼────────────┤  │
│  │  Additive   │  Physical   │  Granular   │  (Future)  │  │
│  │             │  Modeling   │             │            │  │
│  └─────────────┴─────────────┴─────────────┴────────────┘  │
└─────────────────────────────────────────────────────────────┘
                              │
                              ↓
┌─────────────────────────────────────────────────────────────┐
│              Generative & Control Systems                    │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  Randomization Engine                                │  │
│  │  - Seed-based deterministic generation               │  │
│  │  - Lock preservation                                 │  │
│  │  - Constraint validation                             │  │
│  └──────────────────────────────────────────────────────┘  │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  Mutation Engine                                     │  │
│  │  - 2D pad control (intensity × character)            │  │
│  │  - Parameter delta calculation                       │  │
│  └──────────────────────────────────────────────────────┘  │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  Vibe/Macro System (8 macros)                       │  │
│  │  - Timbre, Motion, Space, Texture                   │  │
│  │  - Brightness, Warmth, Grit, Complexity             │  │
│  │  - Multi-parameter mapping                          │  │
│  └──────────────────────────────────────────────────────┘  │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  Modulation System                                   │  │
│  │  - LFOs, envelope followers                          │  │
│  │  - MIDI CC mapping                                   │  │
│  │  - Macro routing                                     │  │
│  └──────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
                              │
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                 Library & Asset Management                   │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  Built-in Factory Library                            │  │
│  │  - Curated, licensed samples                         │  │
│  │  - Organized by category & role                      │  │
│  └──────────────────────────────────────────────────────┘  │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  User Library                                        │  │
│  │  - Import WAV/AIFF                                   │  │
│  │  - Metadata & categorization                         │  │
│  │  - Background loading (not in audio thread)          │  │
│  └──────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
                              │
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                   Effects & Processing                       │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  Per-Layer Effects                                   │  │
│  │  - EQ, filters, saturation, distortion               │  │
│  └──────────────────────────────────────────────────────┘  │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  Master Effects                                      │  │
│  │  - Chorus, phaser, flanger, delay, reverb           │  │
│  │  - Stereo width, compression                         │  │
│  │  - Output gain & clipping protection                 │  │
│  └──────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
                              │
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                    User Interface (JUCE)                     │
│  - Top bar: branding, prompt/seed, randomize, mutate        │
│  - Four layer panels (A, B, C, D)                           │
│  - Bottom macro panel (8 knobs)                             │
│  - Real-time visualizations                                 │
│  - Custom components matching reference design              │
└─────────────────────────────────────────────────────────────┘
```

## Data Flow

### Audio Thread (Real-time Safe)
```
MIDI Event → Voice Manager → Layer Engines → Effects → Output Buffer
                    ↑                ↑
              Voice State      Parameter Values
                              (atomic/lock-free)
```

### UI Thread (Non-real-time)
```
User Input → Parameter Change → Atomic Transfer → Audio Thread
Randomize → Patch Generation → Validation → Safe Transfer → Audio Thread
Sample Load → Background Thread → Preparation → Swap Pointer → Audio Thread
```

## Thread Safety Strategy

1. **Audio Callback**: Lock-free, wait-free parameter reads via atomics
2. **UI Thread**: Can block, performs validation and preparation
3. **Background Threads**: Sample loading, preset serialization, AI (future)
4. **Communication**: 
   - UI → Audio: Atomic parameters, lock-free queues
   - Audio → UI: Atomic state, meters via lock-free buffers

## Voice Architecture

### Global Voice Pool
- **32 simultaneous MIDI notes** (not per layer)
- Each active note can trigger up to 4 layers (A, B, C, D)
- Maximum 128 layer-level voices active (32 notes × 4 layers)

### Voice Stealing Algorithm
1. Check for voices in release phase → steal oldest release
2. If no release voices, find quietest voice
3. Protect recently triggered notes (< 50ms)
4. Apply gentle envelope fade to prevent clicks

### MIDI Handling
- Note on/off with velocity
- Sustain pedal (CC64)
- Pitch bend (±2 semitones default)
- Mod wheel (CC1)
- Expression (CC11)
- All notes off / panic

## Memory Management

### Audio Thread
- Pre-allocated buffers for all voices
- Fixed-size wavetables and lookup tables
- No dynamic allocation in processBlock()

### Sample Loading
- Background thread loads samples
- Atomic pointer swap when ready
- Reference counting for safe deallocation
- Memory pool for sample buffers

### Preset System
- JSON metadata (version, name, category, seed)
- Binary blob for engine states
- Separate sample references (not embedded)
- Versioned serialization for forward compatibility

## File Organization

```
Aetheria/
├── Source/
│   ├── Plugin/
│   │   ├── PluginProcessor.h/cpp
│   │   └── PluginEditor.h/cpp
│   ├── Audio/
│   │   ├── VoiceManager.h/cpp
│   │   ├── Voice.h/cpp
│   │   ├── LayerEngine.h/cpp
│   │   └── AudioEngine.h/cpp
│   ├── Synthesis/
│   │   ├── SampleEngine.h/cpp
│   │   ├── WavetableEngine.h/cpp
│   │   ├── SubtractiveEngine.h/cpp
│   │   ├── FMEngine.h/cpp
│   │   ├── AdditiveEngine.h/cpp
│   │   ├── PhysicalModelEngine.h/cpp
│   │   └── GranularEngine.h/cpp
│   ├── DSP/
│   │   ├── Envelope.h/cpp
│   │   ├── Filter.h/cpp
│   │   ├── Oscillator.h/cpp
│   │   └── Effects/
│   ├── Generation/
│   │   ├── RandomizationEngine.h/cpp
│   │   ├── MutationEngine.h/cpp
│   │   ├── SeedManager.h/cpp
│   │   └── SoundDNA.h/cpp
│   ├── Library/
│   │   ├── BuiltInLibrary.h/cpp
│   │   ├── UserLibrary.h/cpp
│   │   ├── SampleImporter.h/cpp
│   │   └── SampleMetadata.h/cpp
│   ├── Modulation/
│   │   ├── MacroSystem.h/cpp
│   │   ├── LFO.h/cpp
│   │   └── ModulationRouter.h/cpp
│   ├── Presets/
│   │   ├── PresetManager.h/cpp
│   │   └── StateSerializer.h/cpp
│   ├── UI/
│   │   ├── AetheriaLookAndFeel.h/cpp
│   │   ├── TopBar.h/cpp
│   │   ├── LayerPanel.h/cpp
│   │   ├── MacroPanel.h/cpp
│   │   └── Components/
│   └── Utilities/
│       ├── Constants.h
│       ├── Parameters.h
│       └── RealTimeSafety.h
├── Assets/
│   ├── Reference/
│   │   └── Aetheria_UI_Reference.png
│   ├── Fonts/
│   ├── FactoryLibrary/
│   └── Presets/
├── Tests/
│   ├── AudioTests/
│   ├── EngineTests/
│   └── GenerationTests/
├── Documentation/
├── CMakeLists.txt
└── README.md
```

## Build System

### CMake Structure
- Root CMakeLists.txt: Project config, JUCE integration
- JUCE modules automatically linked
- VST3 format enabled
- C++20 standard enforced
- MSVC-specific optimizations

### Build Configurations
- **Debug**: Assertions enabled, optimization off, debug symbols
- **Release**: Full optimization, no assertions, stripped symbols
- **RelWithDebInfo**: Optimized + debug info for profiling

## Performance Targets

- **CPU Usage**: < 10% on modern CPU (4-core i7+) at full polyphony
- **Latency**: < 10ms additional latency beyond host buffer
- **Memory**: < 500 MB for plugin + factory library
- **Load Time**: < 3 seconds for complex preset with samples

## Testing Strategy

### Unit Tests
- Voice allocation logic
- Synthesis engine output validation
- Envelope shape correctness
- Filter frequency response

### Integration Tests
- Full voice lifecycle with MIDI
- Preset save/load round-trips
- Randomization determinism
- Parameter automation

### Real-world Tests
- 32-voice polyphony stress test
- Rapid note triggering
- Sustain pedal behavior
- Plugin state in DAW project

## Future Extensibility

### Planned Additions
- Motion sequencer (Phase 7)
- Arpeggiator (Phase 7)
- Optional AI assistance (Phase 9)
- Additional synthesis engines (Phase 6+)

### Architecture Supports
- New engine types via interface
- Additional macro parameters
- Extended modulation routing
- Plugin preset format versioning

## Security & Stability

### Input Validation
- All user inputs validated and clamped
- Sample file format verification
- Preset file corruption handling
- MIDI event sanitization

### Error Handling
- Graceful degradation on missing samples
- Fallback to silence on engine errors
- Safe state recovery on load failure
- Clear error messages to user

### Resource Limits
- Maximum voice count enforcement
- Sample memory budget
- Preset file size limits
- Undo/redo history depth

---

**Document Version**: 0.1.0  
**Last Updated**: 2026-11-19  
**Status**: Living document - will be updated as architecture evolves
