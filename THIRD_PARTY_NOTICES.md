# Third-Party Notices and Licenses

This document contains licensing information for third-party software used in Aetheria.

## JUCE Framework

**Version**: TBD (7.0+)  
**License**: Dual-licensed (GPL v3 / Commercial)  
**Website**: https://juce.com/  
**Copyright**: © Raw Material Software Limited

### License Summary
JUCE is available under two licenses:
1. **GPL v3**: Free for open-source projects under GPL
2. **Commercial License**: Required for proprietary/commercial products like Aetheria

**Aetheria's License Status**: Commercial license required before commercial release.

### JUCE Modules Used
- juce_audio_basics
- juce_audio_devices
- juce_audio_formats
- juce_audio_plugin_client
- juce_audio_processors
- juce_audio_utils
- juce_core
- juce_data_structures
- juce_dsp
- juce_events
- juce_graphics
- juce_gui_basics
- juce_gui_extra

---

## VST3 SDK

**License**: Steinberg VST3 License  
**Website**: https://www.steinberg.net/vst3sdk/  
**Copyright**: © Steinberg Media Technologies GmbH

### License Summary
The VST3 SDK is included with JUCE. VST3 plugins can be distributed royalty-free under Steinberg's license terms.

**Key Terms**:
- Free to develop and distribute VST3 plugins
- Must include Steinberg's licensing text in documentation
- "VST" is a trademark of Steinberg Media Technologies GmbH

---

## CMake

**Version**: 3.15+  
**License**: BSD 3-Clause License  
**Website**: https://cmake.org/

Build system tool - not distributed with the plugin.

---

## Future Dependencies

As development progresses, additional dependencies will be documented here:

### Testing Frameworks (Planned)
- **Catch2** or **Google Test**: Testing framework
  - License: Boost Software License 1.0 / BSD 3-Clause
  - Usage: Development only, not distributed

### Audio DSP Libraries (If Used)
- To be determined based on implementation needs
- All dependencies will be verified for commercial compatibility

---

## Sample Library Licensing

See [SAMPLE_LICENSES.md](SAMPLE_LICENSES.md) for detailed information about factory sample sources and licenses.

**Critical Requirement**: All factory samples must be:
- Originally created by Spaz SsG, OR
- Licensed for commercial redistribution

Free download ≠ redistribution rights. Each sample must be verified before inclusion.

---

## Open Source Reference Code

**Important**: Aetheria does not copy code from open-source synthesizers (Surge XT, FluidSynth, SuperCollider, etc.) without proper authorization. These projects may be studied as references for DSP algorithms and architecture patterns, but no direct code copying occurs.

If any code is adapted or inspired by open-source projects, it will be documented here with proper attribution and license compliance.

---

## Font Licenses

All fonts used in Aetheria's UI will be documented here:

### TBD
- Font name: TBD
- License: TBD
- Usage: UI text rendering

---

## Trademark Notice

**Aetheria** is a product of **Spaz SsG**.

**VST** is a trademark of Steinberg Media Technologies GmbH.

**Windows** is a trademark of Microsoft Corporation.

**FL Studio** is a trademark of Image-Line Software.

---

**Document Version**: 0.1.0  
**Last Updated**: 2026-11-19  
**Status**: Will be updated as dependencies are added

## License Verification Checklist

Before release, verify:
- [ ] JUCE commercial license obtained
- [ ] All factory samples cleared for redistribution
- [ ] All fonts licensed for commercial use
- [ ] All code dependencies compatible with proprietary software
- [ ] VST3 trademark usage compliant
- [ ] "Aetheria" trademark availability checked
- [ ] Third-party attributions included in installer/manual
