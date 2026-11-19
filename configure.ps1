# Aetheria Build Configuration Script
# Phase 0: Environment and Project Foundation
# Last Updated: 2026-11-19

param(
    [Parameter(HelpMessage="Path to JUCE installation (if not in External/JUCE)")]
    [string]$JucePath = "",
    
    [Parameter(HelpMessage="Build configuration: Debug or Release")]
    [ValidateSet("Debug", "Release", "RelWithDebInfo")]
    [string]$Configuration = "Release",
    
    [Parameter(HelpMessage="Clean build directory before configuration")]
    [switch]$Clean = $false,
    
    [Parameter(HelpMessage="Build after configuration")]
    [switch]$Build = $false,
    
    [Parameter(HelpMessage="Install plugin after build")]
    [switch]$Install = $false
)

Write-Host "=======================================" -ForegroundColor Cyan
Write-Host "  Aetheria Build Configuration" -ForegroundColor Cyan
Write-Host "  by Spaz SsG" -ForegroundColor Cyan
Write-Host "=======================================" -ForegroundColor Cyan
Write-Host ""

# Verify we're in the project root
if (-not (Test-Path "CMakeLists.txt")) {
    Write-Host "ERROR: CMakeLists.txt not found. Please run this script from the project root." -ForegroundColor Red
    exit 1
}

# Project paths
$ProjectRoot = Get-Location
$BuildDir = Join-Path $ProjectRoot "Build"

Write-Host "Project Root: $ProjectRoot" -ForegroundColor Gray
Write-Host "Build Directory: $BuildDir" -ForegroundColor Gray
Write-Host ""

# Clean build directory if requested
if ($Clean -and (Test-Path $BuildDir)) {
    Write-Host "Cleaning build directory..." -ForegroundColor Yellow
    Remove-Item -Path $BuildDir -Recurse -Force
    Write-Host "Build directory cleaned." -ForegroundColor Green
    Write-Host ""
}

# Create build directory
if (-not (Test-Path $BuildDir)) {
    Write-Host "Creating build directory..." -ForegroundColor Yellow
    New-Item -Path $BuildDir -ItemType Directory | Out-Null
}

# Change to build directory
Set-Location $BuildDir

# Prepare CMake arguments
$CMakeArgs = @(
    "..",
    "-G", "Visual Studio 17 2022",
    "-A", "x64"
)

# Add JUCE path if specified
if ($JucePath -ne "") {
    $CMakeArgs += "-DJUCE_DIR=$JucePath"
    Write-Host "Using JUCE from: $JucePath" -ForegroundColor Gray
} else {
    $ExternalJuce = Join-Path $ProjectRoot "External\JUCE"
    if (Test-Path (Join-Path $ExternalJuce "CMakeLists.txt")) {
        Write-Host "Using JUCE from: $ExternalJuce" -ForegroundColor Gray
    } else {
        Write-Host "WARNING: JUCE not found in External/JUCE" -ForegroundColor Yellow
        Write-Host "CMake will fail if JUCE is not installed." -ForegroundColor Yellow
        Write-Host ""
        Write-Host "To install JUCE, run:" -ForegroundColor Yellow
        Write-Host "  git clone --branch 7.0.12 https://github.com/juce-framework/JUCE.git External\JUCE" -ForegroundColor White
        Write-Host ""
    }
}

# Run CMake configuration
Write-Host "Configuring with CMake..." -ForegroundColor Yellow
Write-Host "Command: cmake $($CMakeArgs -join ' ')" -ForegroundColor Gray
Write-Host ""

$CMakeProcess = Start-Process -FilePath "cmake" -ArgumentList $CMakeArgs -NoNewWindow -Wait -PassThru

if ($CMakeProcess.ExitCode -ne 0) {
    Write-Host ""
    Write-Host "ERROR: CMake configuration failed with exit code $($CMakeProcess.ExitCode)" -ForegroundColor Red
    Write-Host ""
    Write-Host "Common issues:" -ForegroundColor Yellow
    Write-Host "  1. JUCE not installed - see message above" -ForegroundColor Gray
    Write-Host "  2. CMake not in PATH" -ForegroundColor Gray
    Write-Host "  3. Visual Studio 2022 not installed" -ForegroundColor Gray
    Write-Host ""
    Set-Location $ProjectRoot
    exit $CMakeProcess.ExitCode
}

Write-Host ""
Write-Host "CMake configuration successful!" -ForegroundColor Green
Write-Host ""

# Build if requested
if ($Build) {
    Write-Host "Building Aetheria ($Configuration)..." -ForegroundColor Yellow
    Write-Host ""
    
    $BuildArgs = @(
        "--build", ".",
        "--config", $Configuration
    )
    
    $BuildProcess = Start-Process -FilePath "cmake" -ArgumentList $BuildArgs -NoNewWindow -Wait -PassThru
    
    if ($BuildProcess.ExitCode -ne 0) {
        Write-Host ""
        Write-Host "ERROR: Build failed with exit code $($BuildProcess.ExitCode)" -ForegroundColor Red
        Set-Location $ProjectRoot
        exit $BuildProcess.ExitCode
    }
    
    Write-Host ""
    Write-Host "Build successful!" -ForegroundColor Green
    Write-Host ""
    
    # Show artifact location
    $ArtifactPath = Join-Path $BuildDir "Aetheria_artefacts\$Configuration\VST3\Aetheria.vst3"
    if (Test-Path $ArtifactPath) {
        Write-Host "Plugin built at:" -ForegroundColor Green
        Write-Host "  $ArtifactPath" -ForegroundColor White
        Write-Host ""
    }
    
    # Install if requested
    if ($Install) {
        Write-Host "Installing plugin to VST3 folder..." -ForegroundColor Yellow
        Write-Host "NOTE: This requires administrator privileges." -ForegroundColor Gray
        Write-Host ""
        
        $InstallArgs = @(
            "--install", ".",
            "--config", $Configuration
        )
        
        $InstallProcess = Start-Process -FilePath "cmake" -ArgumentList $InstallArgs -Verb RunAs -Wait -PassThru
        
        if ($InstallProcess.ExitCode -ne 0) {
            Write-Host ""
            Write-Host "ERROR: Installation failed with exit code $($InstallProcess.ExitCode)" -ForegroundColor Red
            Write-Host "You can manually copy the plugin to:" -ForegroundColor Yellow
            Write-Host "  C:\Program Files\Common Files\VST3\" -ForegroundColor White
            Set-Location $ProjectRoot
            exit $InstallProcess.ExitCode
        }
        
        Write-Host ""
        Write-Host "Plugin installed successfully!" -ForegroundColor Green
        Write-Host "Location: C:\Program Files\Common Files\VST3\Aetheria.vst3" -ForegroundColor White
        Write-Host ""
        Write-Host "Next steps:" -ForegroundColor Cyan
        Write-Host "  1. Open FL Studio 25" -ForegroundColor Gray
        Write-Host "  2. Go to Options > File Settings > Manage Plugins" -ForegroundColor Gray
        Write-Host "  3. Click 'Refresh' or 'Find Plugins'" -ForegroundColor Gray
        Write-Host "  4. Look for 'Aetheria' by 'Spaz SsG'" -ForegroundColor Gray
        Write-Host ""
    }
}

# Return to project root
Set-Location $ProjectRoot

Write-Host "Done!" -ForegroundColor Green
Write-Host ""

# Summary
Write-Host "=======================================" -ForegroundColor Cyan
Write-Host "Configuration Summary:" -ForegroundColor Cyan
Write-Host "  Configuration: $Configuration" -ForegroundColor White
Write-Host "  Build: $(if ($Build) { 'Yes' } else { 'No' })" -ForegroundColor White
Write-Host "  Install: $(if ($Install) { 'Yes' } else { 'No' })" -ForegroundColor White
Write-Host "=======================================" -ForegroundColor Cyan
Write-Host ""

if (-not $Build) {
    Write-Host "To build the plugin, run:" -ForegroundColor Yellow
    Write-Host "  .\configure.ps1 -Build" -ForegroundColor White
    Write-Host ""
    Write-Host "Or open the solution in Visual Studio:" -ForegroundColor Yellow
    Write-Host "  Build\Aetheria.sln" -ForegroundColor White
    Write-Host ""
}

exit 0
