# Build Instructions

Complete instructions for building Aetheria from source on Windows.

## Prerequisites

### Required Software

1. **Windows 11 x64**
   - Windows 10 may work but is not officially supported

2. **Visual Studio 2022**
   - Community, Professional, or Enterprise edition
   - Required workload: "Desktop development with C++"
   - Ensure MSVC v143 compiler is installed
   - Download: https://visualstudio.microsoft.com/

3. **CMake 3.15 or higher**
   - Download: https://cmake.org/download/
   - Add CMake to system PATH during installation
   - Verify: Open command prompt and run `cmake --version`

4. **Git**
   - For version control
   - Download: https://git-scm.com/download/win

5. **JUCE Framework**
   - Required version: 7.0 or higher
   - Download: https://juce.com/get-juce/download
   - See "JUCE Setup" section below

### Disk Space Requirements

- **Minimum**: 5 GB free space
- **Recommended**: 10+ GB for build artifacts and samples

---

## JUCE Setup

### Option 1: Using JUCE with Projucer (Recommended for Development)

1. Download JUCE from https://juce.com/get-juce/download
2. Extract to a permanent location (e.g., `C:\JUCE`)
3. Note the path to the JUCE folder - you'll need it for CMake

### Option 2: Using JUCE as CMake Subdirectory

1. Clone JUCE into the project:
   ```
   cd D:\Aetheria Plugin
   git clone --branch 7.0.0 https://github.com/juce-framework/JUCE.git External/JUCE
   ```

### JUCE Licensing

**Important**: JUCE requires a commercial license for proprietary products like Aetheria.

- **During Development**: You can use JUCE under GPL v3
- **Before Commercial Release**: Obtain a JUCE commercial license from https://juce.com/get-juce

---

## Building Aetheria

### Step 1: Clone the Repository

```batch
git clone <repository-url> "D:\Aetheria Plugin"
cd "D:\Aetheria Plugin"
```

If you already have the source, navigate to the project directory:
```batch
cd "D:\Aetheria Plugin"
```

### Step 2: Configure with CMake

Open a command prompt or PowerShell in the project directory:

```batch
mkdir Build
cd Build
cmake .. -G "Visual Studio 17 2022" -A x64 -DJUCE_DIR="C:\Path\To\JUCE"
```

**Replace** `C:\Path\To\JUCE` with your actual JUCE installation path.

**Example**:
```batch
cmake .. -G "Visual Studio 17 2022" -A x64 -DJUCE_DIR="C:\JUCE"
```

If JUCE is in the `External/JUCE` subdirectory, you can omit `-DJUCE_DIR`:
```batch
cmake .. -G "Visual Studio 17 2022" -A x64
```

### Step 3: Build the Plugin

#### Option A: Build from Command Line

**Debug Build**:
```batch
cmake --build . --config Debug
```

**Release Build**:
```batch
cmake --build . --config Release
```

#### Option B: Build from Visual Studio

1. Open `Build\Aetheria.sln` in Visual Studio 2022
2. Select build configuration (Debug or Release) from toolbar
3. Press `F7` or go to `Build > Build Solution`

### Step 4: Locate the Built Plugin

After successful build, the VST3 plugin will be located at:

**Debug**:
```
D:\Aetheria Plugin\Build\Aetheria_artefacts\Debug\VST3\Aetheria.vst3
```

**Release**:
```
D:\Aetheria Plugin\Build\Aetheria_artefacts\Release\VST3\Aetheria.vst3
```

---

## Installing the Plugin

### Automatic Installation (Recommended)

CMake can copy the plugin to the standard VST3 folder:

```batch
cmake --install . --config Release
```

This copies the plugin to:
```
C:\Program Files\Common Files\VST3\Aetheria.vst3
```

**Note**: Requires administrator privileges.

### Manual Installation

1. Locate the built plugin (see Step 4 above)
2. Copy the entire `Aetheria.vst3` folder to:
   ```
   C:\Program Files\Common Files\VST3\
   ```
3. Or copy to your DAW's custom VST3 folder

---

## Verifying the Plugin in FL Studio 25

1. Open FL Studio 25
2. Go to `Options > File Settings > Manage Plugins`
3. Click "Refresh" or "Find Plugins"
4. FL Studio will scan the VST3 folder
5. Look for "Aetheria" by "Spaz SsG" in the plugin list
6. In the mixer or channel rack, select Aetheria from the plugin browser

---

## Troubleshooting

### CMake Configuration Issues

**Error: "CMake not found"**
- Ensure CMake is installed and added to PATH
- Restart your command prompt after installation

**Error: "Could not find JUCE"**
- Verify JUCE_DIR path is correct
- Ensure JUCE is properly extracted/cloned
- Use absolute paths (e.g., `C:\JUCE`) not relative paths

**Error: "Visual Studio 2022 not found"**
- Install Visual Studio 2022 with C++ workload
- Ensure MSVC v143 toolset is installed

### Build Errors

**Error: "LINK : fatal error LNK1104: cannot open file"**
- Close FL Studio and other DAWs that might be loading the plugin
- Plugins cannot be overwritten while in use

**Error: "MSB8066: Custom build for ... exited with code 1"**
- Check the full build output for specific errors
- Common causes: missing JUCE modules, incorrect paths

**JUCE Module Errors**
- Verify JUCE version is 7.0 or higher
- Ensure all required JUCE modules are present in JUCE installation

### Plugin Loading Issues in FL Studio

**Plugin doesn't appear in FL Studio**
- Verify plugin is in `C:\Program Files\Common Files\VST3\`
- Re-scan plugins in FL Studio settings
- Check FL Studio's plugin manager for errors

**Plugin crashes on load**
- Build in Debug mode: `cmake --build . --config Debug`
- Attach Visual Studio debugger to FL Studio process
- Check Debug output for error messages

**Plugin appears but shows black/empty window**
- UI may not be implemented yet (expected in early development)
- Check development status documentation

---

## Build Configurations

### Debug
- Optimizations disabled
- Debug symbols included
- Assertions enabled
- Slower performance, easier debugging
- Use for development

### Release
- Full optimizations enabled
- Debug symbols stripped
- Assertions disabled
- Best performance
- Use for distribution and performance testing

### RelWithDebInfo
- Optimizations enabled
- Debug symbols included
- Good for profiling

Build with: `cmake --build . --config RelWithDebInfo`

---

## Clean Build

To clean build artifacts and rebuild from scratch:

```batch
cd "D:\Aetheria Plugin"
rmdir /s /q Build
mkdir Build
cd Build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

---

## Development Workflow

1. Make code changes in `Source/` directory
2. Rebuild: `cmake --build Build --config Debug`
3. Close FL Studio if it's running
4. Install updated plugin: `cmake --install Build --config Debug`
5. Reopen FL Studio and test

**Tip**: Use Debug builds during development for better error messages.

---

## System Requirements

### For Building
- Windows 11 x64
- Visual Studio 2022
- CMake 3.15+
- 5-10 GB disk space

### For Running the Plugin
- Windows 11 x64 (Windows 10 may work)
- VST3-compatible DAW (FL Studio 25 tested)
- Reasonably modern CPU (i5/Ryzen 5 or better recommended)
- 4 GB+ RAM

---

## Additional Resources

- JUCE Documentation: https://docs.juce.com/
- JUCE Forum: https://forum.juce.com/
- VST3 Specification: https://steinbergmedia.github.io/vst3_doc/

---

**Document Version**: 0.1.0  
**Last Updated**: 2026-11-19  
**Status**: Will be updated as build system evolves
