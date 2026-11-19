@echo off
REM Setup script for Aetheria project structure
REM Phase 0: Environment and Project Foundation
REM Last Updated: 2026-11-19

echo.
echo ===============================================
echo   Aetheria Directory Setup
echo   by Spaz SsG
echo ===============================================
echo.

REM Create main directories
echo Creating Source directories...
mkdir Source 2>nul
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

echo Creating Assets directories...
mkdir Assets 2>nul
mkdir Assets\Reference 2>nul
mkdir Assets\Fonts 2>nul
mkdir Assets\FactoryLibrary 2>nul
mkdir Assets\Presets 2>nul

echo Creating Tests directories...
mkdir Tests 2>nul
mkdir Tests\AudioTests 2>nul
mkdir Tests\EngineTests 2>nul
mkdir Tests\GenerationTests 2>nul

echo Creating Documentation directories...
mkdir Documentation 2>nul

echo Creating External dependencies directory...
mkdir External 2>nul

echo.
echo ===============================================
echo   Directory structure created successfully!
echo ===============================================
echo.
echo NEXT STEPS:
echo.
echo 1. Place UI reference image at:
echo    Assets\Reference\Aetheria_UI_Reference.png
echo.
echo 2. Install JUCE framework:
echo    git clone --branch 7.0.12 https://github.com/juce-framework/JUCE.git External\JUCE
echo.
echo 3. Configure and build:
echo    PowerShell: .\configure.ps1 -Build
echo    Or manually: See BUILD_INSTRUCTIONS.md
echo.
echo ===============================================
echo.
pause
