# Aetheria
**Infinite Sound. Unlimited Imagination.**

![Status](https://img.shields.io/badge/Status-Phase%200%20Complete-green)
![Platform](https://img.shields.io/badge/Platform-Windows%2011%20x64-blue)
![Format](https://img.shields.io/badge/Format-VST3-orange)

## Overview

**Aetheria** by **Spaz SsG** is a professional generative hybrid software synthesizer designed for both production and experimental sound design. Create unique, playable instruments by combining multiple synthesis technologies across four musical-role layers.

- **Category**: Generative Hybrid Software Synthesizer
- **Platform**: Windows 11 x64
- **Format**: VST3 plugin
- **Target DAW**: FL Studio 25 (initial)
- **Technology**: C++20, JUCE Framework, CMake
- **Compiler**: MSVC/Visual Studio 2022

## Key Features (Planned)

### Four Musical-Role Layers
- **Layer A**: Sub/Body - Foundation and body
- **Layer B**: Mid/Harmony - Harmonic content
- **Layer C**: Attack/Transient - Transient and impact
- **Layer D**: Air/Texture - Atmospheric texture

**Example combination**: Felt piano sample + physical-modeling strings + glass pluck + granular atmosphere

### Synthesis Engines
- **Sample Engine**: Multi-sampling, velocity layers, round robins
- **Wavetable**: Scanning, modulation, unison
- **Subtractive**: Classic analog-style synthesis
- **FM**: Operators, algorithms, feedback
- **Additive**: Harmonic shaping
- **Physical Modeling**: Plucked strings, resonant bodies
- **Granular**: Texture synthesis, grain manipulation

### Generative Sound Design
- **Seed-based randomization**: Deterministic patch generation
- **2D Mutation pad**: Intensity × character control
- **Lock system**: Preserve specific layers/parameters
- **Sound DNA**: Structured patch identity for exploration

### Vibe/Macro System
Eight macro controls for intuitive sound shaping:
- Timbre, Motion, Space, Texture
- Brightness, Warmth, Grit, Complexity

### Audio Performance
- **32 simultaneous MIDI voices** (global, not per-layer)
- **Real-time safe**: No allocations in audio callback
- **Professional quality**: Clean envelopes, anti-aliasing, voice stealing

### Built-in & User Libraries
- Curated factory sample library (licensed for redistribution)
- User sample import (WAV/AIFF)
- Sample browser with categorization

## Quick Start

### Prerequisites
1. Windows 11 x64
2. Visual Studio 2022 (with C++ workload)
3. CMake 3.15+
4. JUCE Framework 7.0+
5. 409 GB available disk space at `D:\Aetheria Plugin`

### Setup Instructions

1. **Clone or extract the repository** to `D:\Aetheria Plugin`

2. **Run directory setup**:
   ```batch
   cd "D:\Aetheria Plugin"
   setup_directories.bat
   ```

3. **Place your UI reference image**:
   - Copy `Aetheria_UI_Reference.png` to `Assets\Reference\`

4. **Install JUCE Framework**:
   
   **Option A - Clone into project** (Recommended):
   ```batch
   cd "D:\Aetheria Plugin"
   git clone --branch 7.0.12 https://github.com/juce-framework/JUCE.git External\JUCE
   ```
   
   **Option B - Use existing JUCE installation**:
   - Note the path to your JUCE folder (e.g., `C:\JUCE`)
   - You'll specify it during CMake configuration

5. **Configure with CMake**:
   ```batch
   mkdir Build
   cd Build
   
   REM If JUCE is in External\JUCE:
   cmake .. -G "Visual Studio 17 2022" -A x64
   
   REM Or if using external JUCE installation:
   cmake .. -G "Visual Studio 17 2022" -A x64 -DJUCE_DIR="C:\Path\To\JUCE"
   ```

6. **Build the plugin**:
   ```batch
   cmake --build . --config Release
   ```

7. **Install to VST3 folder**:
   ```batch
   cmake --install . --config Release
   ```

8. **Load in FL Studio 25**:
   - Open FL Studio
   - Go to Options > File Settings > Manage Plugins
   - Click "Refresh" or "Find Plugins"
   - Look for "Aetheria" by "Spaz SsG"

For detailed build instructions, see [BUILD_INSTRUCTIONS.md](BUILD_INSTRUCTIONS.md).

## Documentation

- **[DEVELOPMENT_STATUS.md](DEVELOPMENT_STATUS.md)** - Current development progress
- **[TECHNICAL_ARCHITECTURE.md](TECHNICAL_ARCHITECTURE.md)** - System architecture and design
- **[BUILD_INSTRUCTIONS.md](BUILD_INSTRUCTIONS.md)** - Detailed build guide
- **[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)** - Licensing information
- **[SAMPLE_LICENSES.md](SAMPLE_LICENSES.md)** - Sample library licensing
- **[CHANGELOG.md](CHANGELOG.md)** - Version history

## Development Roadmap

- [x] **Phase 0**: Environment & Foundation ✅
- [ ] **Phase 1**: Plugin Foundation
- [ ] **Phase 2**: Official GUI Recreation
- [ ] **Phase 3**: Sample Engine & Library
- [ ] **Phase 4**: Generative Engine
- [ ] **Phase 5**: Mutation & Sound DNA
- [ ] **Phase 6**: Advanced Synthesis Engines
- [ ] **Phase 7**: Macros & Modulation
- [ ] **Phase 8**: Effects & Presets
- [ ] **Phase 9**: Optional AI Foundation
- [ ] **Phase 10**: QA & Release Preparation

## Brand Identity

**Colors**:
- Near Black: `#08090C`
- Electric Cyan: `#00DDF5`
- Warm Orange: `#FF791D`
- Silver: `#D9DDE2`

**Visual Style**: Premium dark metallic hardware aesthetic with controlled illumination and professional controls.

## Licensing

### JUCE Framework
Aetheria requires a **JUCE Commercial License** for commercial distribution. During development, JUCE can be used under GPL v3.

- JUCE: https://juce.com/get-juce
- Pricing: https://juce.com/get-juce/pricing

### VST3
VST3 plugins can be distributed royalty-free under Steinberg's license. "VST" is a trademark of Steinberg Media Technologies GmbH.

### Aetheria
Copyright © 2024 Spaz SsG. All rights reserved.

See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) for complete licensing information.

## Current Status

**Phase 0 Complete** ✅
- Project structure established
- CMake build system configured
- Initial VST3 plugin scaffold created
- Documentation framework in place

**Next Milestone**: Phase 1 - Implement voice management and basic audio processing.

## Contributing

Aetheria is a proprietary commercial product. This repository is for development by Spaz SsG and authorized contributors only.

## Support

For technical questions or issues during development, refer to:
- JUCE Documentation: https://docs.juce.com/
- JUCE Forum: https://forum.juce.com/

---

**Aetheria** by **Spaz SsG** | Version 0.1.0-alpha | Last Updated: 2026-11-19
