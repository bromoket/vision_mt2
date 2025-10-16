@echo off
echo Checking Vision External Dependencies...
echo.

echo Checking if executable exists...
if exist "build\bin\vision_external.exe" (
    echo [OK] vision_external.exe found
) else (
    echo [ERROR] vision_external.exe not found in build\bin\
    echo Please build the project first
    pause
    exit /b 1
)

echo.
echo Checking DirectX 9 installation...
if exist "%SystemRoot%\System32\d3d9.dll" (
    echo [OK] d3d9.dll found in System32
) else (
    echo [WARNING] d3d9.dll not found - DirectX 9 may not be installed
)

echo.
echo Checking Visual C++ Redistributables...
reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x86" >nul 2>&1
if %errorlevel% equ 0 (
    echo [OK] Visual C++ 2015-2019 x86 Redistributable found
) else (
    echo [WARNING] Visual C++ 2015-2019 x86 Redistributable may not be installed
)

echo.
echo Running dependency walker simulation...
echo Checking for common missing DLLs...

set "MISSING_DLLS="

if not exist "%SystemRoot%\System32\msvcp140.dll" (
    set "MISSING_DLLS=%MISSING_DLLS% msvcp140.dll"
)

if not exist "%SystemRoot%\System32\vcruntime140.dll" (
    set "MISSING_DLLS=%MISSING_DLLS% vcruntime140.dll"
)

if not exist "%SystemRoot%\System32\api-ms-win-crt-runtime-l1-1-0.dll" (
    set "MISSING_DLLS=%MISSING_DLLS% api-ms-win-crt-runtime-l1-1-0.dll"
)

if "%MISSING_DLLS%"=="" (
    echo [OK] All common runtime DLLs found
) else (
    echo [ERROR] Missing DLLs:%MISSING_DLLS%
    echo Please install Visual C++ Redistributable 2015-2019 x86
)

echo.
echo Attempting to run vision_external.exe...
echo If it exits immediately, check the log file: vision_external.log
echo.

cd /d "%~dp0"
if exist "build\bin\vision_external.exe" (
    "build\bin\vision_external.exe"
) else (
    echo Cannot find executable to run
)

echo.
echo Check complete. Press any key to exit...
pause >nul
