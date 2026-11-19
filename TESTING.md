# Aetheria Testing Documentation

**Project**: Aetheria by Spaz SsG  
**Version**: 0.1.0-alpha  
**Last Updated**: 2026-11-19

---

## Testing Strategy

Aetheria follows a comprehensive testing approach to ensure professional audio quality, stability, and correctness.

### Testing Levels

1. **Unit Tests**: Individual component validation
2. **Integration Tests**: Multi-component interaction
3. **System Tests**: Full plugin behavior in host
4. **Performance Tests**: CPU, memory, latency profiling
5. **Compatibility Tests**: DAW-specific validation

---

## Phase 0: Foundation Testing

### Acceptance Tests

Phase 0 is complete when:

- [x] CMake configure succeeds
- [ ] Debug build completes without errors
- [ ] Release build completes without errors
- [ ] VST3 artifact (.vst3 bundle) exists at expected path
- [ ] No user files lost during setup
- [ ] Documentation is accurate and complete

### Manual Verification Checklist

```
[ ] CMake configuration runs without fatal errors
[ ] Visual Studio solution generates correctly
[ ] Source files compile without errors
[ ] Linker produces VST3 binary
[ ] VST3 bundle structure is valid
[ ] Plugin loads in FL Studio 25 (or test host)
[ ] Plugin window opens without crash
[ ] Plugin can be closed cleanly
```

---

## Phase 1: Plugin Foundation Testing

### Voice Management Tests (Unit)

```cpp
TEST(VoiceManager, AllocatesUpTo32Voices)
TEST(VoiceManager, StealsOldestReleaseVoice)
TEST(VoiceManager, StealsQuietestVoiceWhenNoRelease)
TEST(VoiceManager, HandlesNoteOffCorrectly)
TEST(VoiceManager, HandlesSustainPedal)
TEST(VoiceManager, NoVoiceLeaksAfterAllNotesOff)
```

### MIDI Handling Tests (Integration)

```cpp
TEST(MidiHandling, ProcessesNoteOn)
TEST(MidiHandling, ProcessesNoteOff)
TEST(MidiHandling, HandlesVelocity)
TEST(MidiHandling, HandlesPitchBend)
TEST(MidiHandling, HandlesModWheel)
TEST(MidiHandling, HandlesAllNotesOff)
```

### Audio Safety Tests (Critical)

```cpp
TEST(AudioCallback, NoAllocationInProcessBlock)
TEST(AudioCallback, NoDiskIOInProcessBlock)
TEST(AudioCallback, NoBlockingMutexInProcessBlock)
TEST(AudioCallback, NoExceptionsThrown)
TEST(AudioCallback, ProducesNonNaNOutput)
```

---

## Phase 3: Synthesis Engine Testing

### Sample Engine Tests

```cpp
TEST(SampleEngine, LoadsWAVFile)
TEST(SampleEngine, LoadsAIFFFile)
TEST(SampleEngine, HandlesMissingSample)
TEST(SampleEngine, PlaysAtCorrectPitch)
TEST(SampleEngine, RespectsVelocityLayers)
TEST(SampleEngine, HandlesLoopPoints)
```

### Wavetable Engine Tests

```cpp
TEST(WavetableEngine, ScansWavetable)
TEST(WavetableEngine, InterpolatesSmooth)
TEST(WavetableEngine, HandlesUnison)
TEST(WavetableEngine, NoAliasingAtNyquist)
```

---

## Phase 4: Generative Engine Testing

### Randomization Tests (Critical for Determinism)

```cpp
TEST(Randomization, SameSeedProducesSamePatch)
TEST(Randomization, DifferentSeedProducesDifferentPatch)
TEST(Randomization, RespectsLockedLayers)
TEST(Randomization, GeneratesValidParameters)
TEST(Randomization, HandlesConstraints)
```

### Mutation Tests

```cpp
TEST(Mutation, LowDepthMakesSmallChanges)
TEST(Mutation, HighDepthMakesLargeChanges)
TEST(Mutation, PreservesLockedParameters)
TEST(Mutation, StaysWithinBounds)
TEST(Mutation, ReproducibleWithSameSeed)
```

---

## Phase 8: Preset System Testing

### Serialization Tests

```cpp
TEST(Presets, SaveAndLoadRoundTrip)
TEST(Presets, HandlesVersionMismatch)
TEST(Presets, HandlesMissingSamples)
TEST(Presets, PreservesAllParameters)
TEST(Presets, HandlesCorruptFile)
```

---

## Testing Infrastructure

### Framework Selection (Phase 1)

**Options under consideration**:
- **Catch2**: Modern C++, header-only, BDD style
- **Google Test**: Industry standard, mature
- **JUCE UnitTest**: Built into JUCE framework

**Decision**: TBD in Phase 1

### Test Organization

```
Tests/
├── AudioTests/           # Audio processing tests
│   ├── VoiceManagerTest.cpp
│   ├── MidiHandlingTest.cpp
│   └── AudioSafetyTest.cpp
├── EngineTests/          # Synthesis engine tests
│   ├── SampleEngineTest.cpp
│   ├── WavetableEngineTest.cpp
│   └── FilterTest.cpp
├── GenerationTests/      # Randomization/mutation tests
│   ├── RandomizationTest.cpp
│   ├── MutationTest.cpp
│   └── SeedTest.cpp
└── TestUtilities/        # Shared test helpers
    ├── AudioTestHelpers.h
    └── MockClasses.h
```

---

## Real-Time Safety Validation

### Sanitizer Integration

**Thread Sanitizer**: Detect race conditions
```powershell
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_TSAN=ON ..
```

**Address Sanitizer**: Detect memory issues
```powershell
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=ON ..
```

### Real-Time Violation Detection

Tools to consider:
- JUCE AudioPluginHost with debug mode
- Manual inspection of processBlock() call tree
- Custom allocation hooks in debug builds

---

## Performance Testing

### CPU Profiling

**Scenarios**:
1. 32-voice polyphony, all layers active
2. Complex preset with all effects
3. Rapid note triggering (stress test)
4. Continuous parameter automation

**Target**: < 10% CPU on modern i7 @ 44.1kHz, 512 samples buffer

### Memory Profiling

**Metrics**:
- Plugin memory footprint (target: < 500 MB)
- Per-voice memory usage
- Sample library memory usage
- No memory leaks after preset changes

### Latency Testing

**Target**: < 10ms additional latency beyond host buffer

---

## DAW Compatibility Testing

### FL Studio 25 (Primary Target)

**Test Cases**:
- [ ] Plugin loads successfully
- [ ] MIDI notes trigger audio
- [ ] Velocity sensitivity works
- [ ] Pitch bend works
- [ ] Sustain pedal works
- [ ] Automation works
- [ ] Preset save/load in project
- [ ] Plugin state saves with project
- [ ] No crashes during project save/load
- [ ] No audio glitches or dropouts

### Other DAWs (Secondary)

- Ableton Live (test if available)
- Reaper (test if available)
- Cubase (test if available)

---

## Continuous Integration (Future)

### CI Pipeline (Phase 10)

```yaml
# Example CI workflow
build:
  - Configure with CMake
  - Build Debug
  - Build Release
  - Run unit tests
  - Run integration tests
  - Check for memory leaks
  - Generate coverage report
```

---

## Test Execution

### Running Tests (Once Implemented)

**Command Line**:
```powershell
# Build with tests
cmake .. -DBUILD_TESTS=ON

# Run all tests
ctest --output-on-failure

# Run specific test
ctest -R VoiceManagerTest -VV
```

**Visual Studio**:
1. Set test project as startup
2. Press F5 to run under debugger
3. Check Test Explorer window

---

## Coverage Targets

### Phase-by-Phase Goals

- Phase 1: 80%+ coverage of voice/MIDI code
- Phase 3: 70%+ coverage of synthesis engines
- Phase 4: 90%+ coverage of randomization (critical for determinism)
- Phase 8: 80%+ coverage of preset system

### Critical Paths Requiring 100% Coverage

1. Voice allocation/deallocation
2. MIDI note-on/note-off handling
3. Real-time safety critical paths
4. Seed-based randomization logic

---

## Bug Reporting Template

```markdown
**Title**: Brief description

**Phase**: Which development phase

**Severity**: Critical / High / Medium / Low

**Environment**:
- OS: Windows 11
- DAW: FL Studio 25
- Plugin Version: 0.x.x
- Build Config: Debug / Release

**Steps to Reproduce**:
1. Step one
2. Step two
3. ...

**Expected Behavior**: What should happen

**Actual Behavior**: What actually happens

**Logs/Stack Trace**: (if applicable)

**Workaround**: (if known)
```

---

## Phase 0 Status

**Testing Infrastructure**: Not yet implemented (expected)

**Manual Verification**: 
- [x] Project structure created
- [ ] CMake configuration verified (pending JUCE)
- [ ] Build verification (pending JUCE)
- [ ] Plugin loading test (pending JUCE)

**Next Step**: Complete source file creation, then user installs JUCE and performs build verification.

---

**Document Version**: 1.0  
**Last Updated**: 2026-11-19  
**Status**: Testing infrastructure to be implemented starting Phase 1
