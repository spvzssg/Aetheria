@echo off
REM Setup script for Aetheria project structure

echo Setting up Aetheria directory structure...

REM Create main directories
mkdir Source\Plugin 2>nul
mkdir Source\Audio 2>nul
mkdir Source\Synthesis 2>nul
mkdir Source\DSP 2>nul
mkdir Source\DSP\Effects 2>nul
mkdir Source\Generation 2>nul
mkdir Source\Library 2>nul
mkdir Source\Modulation 2>nul
mkdir Source\Presets 2>nul
mkdir Source\UI 2>nul
mkdir Source\UI\Components 2>nul
mkdir Source\Utilities 2>nul

mkdir Assets 2>nul
mkdir Assets\Reference 2>nul
mkdir Assets\Fonts 2>nul
mkdir Assets\FactoryLibrary 2>nul
mkdir Assets\Presets 2>nul

mkdir Tests 2>nul
mkdir Tests\AudioTests 2>nul
mkdir Tests\EngineTests 2>nul
mkdir Tests\GenerationTests 2>nul

mkdir Documentation 2>nul

mkdir External 2>nul

echo.
echo Directory structure created successfully!
echo.
echo IMPORTANT: Place your UI reference image at:
echo   D:\Aetheria Plugin\Assets\Reference\Aetheria_UI_Reference.png
echo.
echo Next steps:
echo   1. Install or clone JUCE framework to External\JUCE
echo   2. Run CMake configuration
echo   3. Build the project
echo.
echo See BUILD_INSTRUCTIONS.md for detailed setup guide.
pause
