@echo off
setlocal enabledelayedexpansion

echo ========================================
echo Vision - Metin2 Game Helper Build Script
echo ========================================
echo.

:: Check if vcpkg is available
if "%VCPKG_ROOT%"=="" (
    echo ERROR: VCPKG_ROOT environment variable not set!
    echo Please set VCPKG_ROOT to your vcpkg installation directory.
    echo Example: set VCPKG_ROOT=C:\vcpkg
    pause
    exit /b 1
)

if not exist "%VCPKG_ROOT%\vcpkg.exe" (
    echo ERROR: vcpkg.exe not found at %VCPKG_ROOT%
    echo Please ensure vcpkg is properly installed.
    pause
    exit /b 1
)

:: Check for ImGui
if not exist "external\imgui\imgui.h" (
    echo WARNING: ImGui not found in external\imgui\
    echo The external project will build without ImGui support.
    echo To enable ImGui, copy ImGui source files to external\imgui\
    echo.
)

:: Install dependencies
echo Installing vcpkg dependencies...
"%VCPKG_ROOT%\vcpkg.exe" install fmt:x86-windows nlohmann-json:x86-windows zydis:x86-windows

if !errorlevel! neq 0 (
    echo ERROR: Failed to install vcpkg dependencies
    pause
    exit /b 1
)

:: Create build directory
if not exist "build" mkdir build
cd build

:: Configure with CMake
echo.
echo Configuring with CMake...
cmake .. -G "Visual Studio 17 2022" -A Win32 -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake"

if !errorlevel! neq 0 (
    echo ERROR: CMake configuration failed
    echo.
    echo Trying alternative generator...
    cmake .. -G "Visual Studio 16 2019" -A Win32 -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake"
    
    if !errorlevel! neq 0 (
        echo ERROR: CMake configuration failed with both generators
        echo Please ensure you have Visual Studio 2019 or 2022 installed
        pause
        exit /b 1
    )
)

:: Build the project
echo.
echo Building project (Release)...
cmake --build . --config Release

if !errorlevel! neq 0 (
    echo ERROR: Build failed
    pause
    exit /b 1
)

echo.
echo ========================================
echo Build completed successfully!
echo ========================================
echo.
echo Output files:
echo - vision_external.exe: Release\vision_external.exe
echo - vision_internal.dll: Release\vision_internal.dll
echo.
echo To debug, open build\vision.sln in Visual Studio
echo.
pause
