#!/usr/bin/env pwsh

Write-Host "Building Vision MT2 (Release Mode)..." -ForegroundColor Cyan
Write-Host ""

# Check if build directory exists and clean if needed
if (Test-Path "build") {
    Write-Host "Cleaning previous build cache..." -ForegroundColor Yellow
    Remove-Item "build" -Recurse -Force
}

# Configure the project with Visual Studio generator
Write-Host "Configuring project..." -ForegroundColor Cyan
& cmake -B build -S . -A Win32

if ($LASTEXITCODE -ne 0) {
    Write-Host "Configuration failed!" -ForegroundColor Red
    Write-Host "Make sure Visual Studio 2019/2022 with C++ tools is installed." -ForegroundColor Yellow
    Read-Host "Press Enter to continue"
    exit $LASTEXITCODE
}

# Build the project in Release mode
Write-Host ""
Write-Host "Building project in Release mode..." -ForegroundColor Cyan
& cmake --build build --config Release

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    Read-Host "Press Enter to continue"
    exit $LASTEXITCODE
}

Write-Host ""
Write-Host "Release build completed successfully!" -ForegroundColor Green
Write-Host "Binaries are in: build\bin\Release\" -ForegroundColor Cyan
Write-Host ""
Read-Host "Press Enter to continue"
