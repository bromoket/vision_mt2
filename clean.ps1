#!/usr/bin/env pwsh

Write-Host "Cleaning Vision MT2 build files..." -ForegroundColor Cyan
Write-Host ""

# Remove build directory
if (Test-Path "build") {
    Write-Host "Removing build directory..." -ForegroundColor Yellow
    Remove-Item "build" -Recurse -Force
    Write-Host "Build directory removed." -ForegroundColor Green
} else {
    Write-Host "Build directory not found." -ForegroundColor Yellow
}

# Remove any CMake cache files in root
$cacheFiles = @("CMakeCache.txt", "CMakeFiles")
foreach ($file in $cacheFiles) {
    if (Test-Path $file) {
        Write-Host "Removing $file..." -ForegroundColor Yellow
        Remove-Item $file -Recurse -Force
    }
}

Write-Host ""
Write-Host "Clean completed!" -ForegroundColor Green
Write-Host ""
Write-Host "Available build commands:" -ForegroundColor Cyan
Write-Host "  .\build.ps1          # Developer build (optimized with debug info)" -ForegroundColor White
Write-Host "  .\build-production.ps1  # Release build (full optimization)" -ForegroundColor White
Write-Host "  .\build-debug.ps1       # Debug build (no optimization, full debug)" -ForegroundColor White
Write-Host ""
