# Vision - Metin2 Game Helper Build Script (PowerShell)
# Requires PowerShell 5.1 or higher

param(
    [string]$Config = "Release",
    [string]$Generator = "Visual Studio 17 2022",
    [switch]$Clean,
    [switch]$Help
)

if ($Help) {
    Write-Host "Vision Build Script" -ForegroundColor Cyan
    Write-Host "Usage: .\build.ps1 [-Config <Debug|Release>] [-Generator <generator>] [-Clean] [-Help]" -ForegroundColor White
    Write-Host ""
    Write-Host "Parameters:" -ForegroundColor Yellow
    Write-Host "  -Config     Build configuration (Debug or Release, default: Release)" -ForegroundColor White
    Write-Host "  -Generator  CMake generator (default: 'Visual Studio 17 2022')" -ForegroundColor White
    Write-Host "  -Clean      Clean build directory before building" -ForegroundColor White
    Write-Host "  -Help       Show this help message" -ForegroundColor White
    Write-Host ""
    Write-Host "Examples:" -ForegroundColor Yellow
    Write-Host "  .\build.ps1                    # Build Release with VS 2022" -ForegroundColor Gray
    Write-Host "  .\build.ps1 -Config Debug      # Build Debug configuration" -ForegroundColor Gray
    Write-Host "  .\build.ps1 -Clean             # Clean and build" -ForegroundColor Gray
    exit 0
}

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Vision - Metin2 Game Helper Build Script" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Function to check if command exists
function Test-Command {
    param([string]$Command)
    try {
        Get-Command $Command -ErrorAction Stop | Out-Null
        return $true
    } catch {
        return $false
    }
}

# Function to write colored output
function Write-Status {
    param([string]$Message, [string]$Status = "INFO")
    switch ($Status) {
        "ERROR" { Write-Host "[ERROR] $Message" -ForegroundColor Red }
        "WARN"  { Write-Host "[WARN]  $Message" -ForegroundColor Yellow }
        "OK"    { Write-Host "[OK]    $Message" -ForegroundColor Green }
        default { Write-Host "[INFO]  $Message" -ForegroundColor Cyan }
    }
}

# Check prerequisites
Write-Status "Checking prerequisites..."

# Check vcpkg
if (-not $env:VCPKG_ROOT) {
    Write-Status "VCPKG_ROOT environment variable not set!" "ERROR"
    Write-Status "Please set VCPKG_ROOT to your vcpkg installation directory." "ERROR"
    Write-Status "Example: `$env:VCPKG_ROOT = 'C:\vcpkg'" "ERROR"
    exit 1
}

if (-not (Test-Path "$env:VCPKG_ROOT\vcpkg.exe")) {
    Write-Status "vcpkg.exe not found at $env:VCPKG_ROOT" "ERROR"
    Write-Status "Please ensure vcpkg is properly installed." "ERROR"
    exit 1
}

Write-Status "Found vcpkg at: $env:VCPKG_ROOT" "OK"

# Check CMake
if (-not (Test-Command "cmake")) {
    Write-Status "CMake not found in PATH!" "ERROR"
    Write-Status "Please install CMake and add it to your PATH." "ERROR"
    exit 1
}

$cmakeVersion = (cmake --version | Select-Object -First 1) -replace "cmake version ", ""
Write-Status "Found CMake version: $cmakeVersion" "OK"

# Check for ImGui
if (-not (Test-Path "external\imgui\imgui.h")) {
    Write-Status "ImGui not found in external\imgui\" "WARN"
    Write-Status "The external project will build without ImGui support." "WARN"
    Write-Status "To enable ImGui, copy ImGui source files to external\imgui\" "WARN"
    Write-Host ""
}

# Clean build directory if requested
if ($Clean -and (Test-Path "build")) {
    Write-Status "Cleaning build directory..."
    Remove-Item -Recurse -Force "build"
    Write-Status "Build directory cleaned" "OK"
}

# Install dependencies
Write-Status "Installing vcpkg dependencies..."
$packages = @("fmt:x86-windows", "nlohmann-json:x86-windows", "zydis:x86-windows")

foreach ($package in $packages) {
    Write-Status "Installing $package..."
    & "$env:VCPKG_ROOT\vcpkg.exe" install $package
    if ($LASTEXITCODE -ne 0) {
        Write-Status "Failed to install $package" "ERROR"
        exit 1
    }
}

Write-Status "All dependencies installed successfully" "OK"

# Create build directory
if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

Set-Location "build"

# Configure with CMake
Write-Status "Configuring with CMake..."
Write-Status "Generator: $Generator"
Write-Status "Configuration: $Config"

$cmakeArgs = @(
    "..",
    "-G", $Generator,
    "-A", "Win32",
    "-DCMAKE_TOOLCHAIN_FILE=$env:VCPKG_ROOT\scripts\buildsystems\vcpkg.cmake"
)

& cmake @cmakeArgs

if ($LASTEXITCODE -ne 0) {
    Write-Status "CMake configuration failed with $Generator" "ERROR"
    
    # Try fallback generator
    Write-Status "Trying fallback generator: Visual Studio 16 2019..."
    $fallbackArgs = @(
        "..",
        "-G", "Visual Studio 16 2019",
        "-A", "Win32",
        "-DCMAKE_TOOLCHAIN_FILE=$env:VCPKG_ROOT\scripts\buildsystems\vcpkg.cmake"
    )
    
    & cmake @fallbackArgs
    
    if ($LASTEXITCODE -ne 0) {
        Write-Status "CMake configuration failed with both generators" "ERROR"
        Write-Status "Please ensure you have Visual Studio 2019 or 2022 installed" "ERROR"
        Set-Location ".."
        exit 1
    }
}

Write-Status "CMake configuration successful" "OK"

# Build the project
Write-Status "Building project ($Config configuration)..."
& cmake --build . --config $Config --parallel

if ($LASTEXITCODE -ne 0) {
    Write-Status "Build failed" "ERROR"
    Set-Location ".."
    exit 1
}

Set-Location ".."

Write-Status "Build completed successfully!" "OK"
Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "Build Summary" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "Configuration: $Config" -ForegroundColor White
Write-Host "Output files:" -ForegroundColor White
Write-Host "  - vision_external.exe: build\$Config\vision_external.exe" -ForegroundColor Gray
Write-Host "  - vision_internal.dll: build\$Config\vision_internal.dll" -ForegroundColor Gray
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Yellow
Write-Host "  1. Copy vision_internal.dll to the same directory as vision_external.exe" -ForegroundColor Gray
Write-Host "  2. Run vision_external.exe to start the application" -ForegroundColor Gray
Write-Host "  3. Open build\vision.sln in Visual Studio for debugging" -ForegroundColor Gray
Write-Host ""
