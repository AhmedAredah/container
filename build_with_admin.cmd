@echo off

:: Check if running with admin privileges
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo Requesting admin privileges...
    powershell -Command "Start-Process '%~f0' -Verb runAs"
    exit /b
)

:: Change to the directory where the script is located
cd /d "%~dp0"

:: Remove existing build directory
rmdir /s /q build

:: Create build directory
mkdir build
cd build

:: Configure and build in Debug mode
cmake .. -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Debug
cmake --build . --target INSTALL --config Debug

:: Configure and build in Release mode
cmake .. -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build . --target INSTALL --config Release

echo Build process completed.
pause
