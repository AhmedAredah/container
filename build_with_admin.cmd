@echo off
setlocal enabledelayedexpansion

:: Function to check command success (implemented through error checking)
set "ERROR_PREFIX=Error:"

:: Change to the directory where the script is located
cd /d "%~dp0"

:: Ensure Conda is available
where conda >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo %ERROR_PREFIX% Conda is not installed or not in PATH.
    exit /b 1
)

:: Ensure an active Conda environment
if "%CONDA_PREFIX%"=="" (
    echo %ERROR_PREFIX% No active Conda environment found. Please activate an Anaconda environment before running the script.
    exit /b 1
)

set "ACTIVE_ENV=%CONDA_PREFIX%"
echo Using Conda environment: %ACTIVE_ENV%

:: Check Python path
where python >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo %ERROR_PREFIX% Python not found in the active Conda environment.
    exit /b 1
)

for /f "tokens=*" %%i in ('where python') do set "PYTHON_PATH=%%i"
echo Using Python: %PYTHON_PATH%

:: Ensure pip is up to date
"%PYTHON_PATH%" -m pip install --no-cache-dir --upgrade pip
if %ERRORLEVEL% neq 0 (
    echo %ERROR_PREFIX% Pip upgrade failed
    exit /b 1
)

:: Install dependencies using pip
echo Installing PyQt6 and pybind11...
"%PYTHON_PATH%" -m pip install --no-cache-dir pybind11 PyQt6
if %ERRORLEVEL% neq 0 (
    echo %ERROR_PREFIX% Pip dependencies installation failed
    exit /b 1
)

:: Set default Qt path for Windows
set "DEFAULT_QT_PATH=C:\Qt\6.8.0\msvc2019_64\lib\cmake\Qt6"

:: Get number of CPU cores
for /f "tokens=2 delims==" %%a in ('wmic cpu get NumberOfCores /value') do set "NUM_CORES=%%a"

:: Ask user for Qt path
set /p "USE_DEFAULT_QT_PATH=Use default Qt path (%DEFAULT_QT_PATH%)? ([y]/n) "
if "%USE_DEFAULT_QT_PATH%"=="" set "USE_DEFAULT_QT_PATH=y"

if /i "%USE_DEFAULT_QT_PATH%"=="y" (
    set "QT_PATH=%DEFAULT_QT_PATH%"
) else (
    set /p "QT_PATH=Enter custom Qt path: "
    if "!QT_PATH!"=="" (
        echo %ERROR_PREFIX% No Qt path provided. Exiting.
        exit /b 1
    )
)
echo Using Qt path: %QT_PATH%

:: Get pybind11 cmake directory
for /f "tokens=*" %%i in ('"%PYTHON_PATH%" -c "import pybind11; print(pybind11.get_cmake_dir())"') do set "PYBIND11_DIR=%%i"

:: Ask the user about the build and installation steps
set /p "BUILD_DEBUG=Do you want to build the Debug version? ([y]/n) "
if "%BUILD_DEBUG%"=="" set "BUILD_DEBUG=y"

set /p "BUILD_RELEASE=Do you want to build the Release version? ([y]/n) "
if "%BUILD_RELEASE%"=="" set "BUILD_RELEASE=y"

set /p "INSTALL_DEBUG=Do you want to install the Debug build? ([y]/n) "
if "%INSTALL_DEBUG%"=="" set "INSTALL_DEBUG=y"

set /p "INSTALL_RELEASE=Do you want to install the Release build? ([y]/n) "
if "%INSTALL_RELEASE%"=="" set "INSTALL_RELEASE=y"

set /p "BUILD_WHL=Do you want to build the Python wheel file? ([y]/n) "
if "%BUILD_WHL%"=="" set "BUILD_WHL=y"

set /p "INSTALL_WHL=Do you want to install the Python wheel file? ([y]/n) "
if "%INSTALL_WHL%"=="" set "INSTALL_WHL=y"

:: Display summary
echo.
echo **Summary of Selected Options:**
echo --------------------------------
echo Using Conda environment: %ACTIVE_ENV%
echo Using Python: %PYTHON_PATH%
echo Using Qt path: %QT_PATH%
echo Build Debug version: %BUILD_DEBUG%
echo Build Release version: %BUILD_RELEASE%
echo Install Debug build: %INSTALL_DEBUG%
echo Install Release build: %INSTALL_RELEASE%
echo Build Python wheel file: %BUILD_WHL%
echo Install Python wheel file: %INSTALL_WHL%
echo --------------------------------

:: Ask if the user wants to modify any selection
set /p "MODIFY_SELECTION=Do you want to modify any selection? (y/[n]) "
if "%MODIFY_SELECTION%"=="" set "MODIFY_SELECTION=n"

if /i "%MODIFY_SELECTION%"=="y" (
    echo.
    echo Re-enter the options you want to change. Press Enter to keep the current selection.

    set /p "NEW_BUILD_DEBUG=Build Debug version? (Current: %BUILD_DEBUG%) ([y]/n) "
    if not "%NEW_BUILD_DEBUG%"=="" set "BUILD_DEBUG=%NEW_BUILD_DEBUG%"

    set /p "NEW_BUILD_RELEASE=Build Release version? (Current: %BUILD_RELEASE%) ([y]/n) "
    if not "%NEW_BUILD_RELEASE%"=="" set "BUILD_RELEASE=%NEW_BUILD_RELEASE%"

    set /p "NEW_INSTALL_DEBUG=Install Debug build? (Current: %INSTALL_DEBUG%) ([y]/n) "
    if not "%NEW_INSTALL_DEBUG%"=="" set "INSTALL_DEBUG=%NEW_INSTALL_DEBUG%"

    set /p "NEW_INSTALL_RELEASE=Install Release build? (Current: %INSTALL_RELEASE%) ([y]/n) "
    if not "%NEW_INSTALL_RELEASE%"=="" set "INSTALL_RELEASE=%NEW_INSTALL_RELEASE%"

    set /p "NEW_BUILD_WHL=Build Python wheel file? (Current: %BUILD_WHL%) ([y]/n) "
    if not "%NEW_BUILD_WHL%"=="" set "BUILD_WHL=%NEW_BUILD_WHL%"

    set /p "NEW_INSTALL_WHL=Install Python wheel file? (Current: %INSTALL_WHL%) ([y]/n) "
    if not "%NEW_INSTALL_WHL%"=="" set "INSTALL_WHL=%NEW_INSTALL_WHL%"
)

echo.
echo Proceeding with the selected options...

:: Remove old build directories
echo Removing old build directories...
if exist build_debug rmdir /s /q build_debug
if exist build_release rmdir /s /q build_release
if exist dist rmdir /s /q dist
del /f /q *.egg-info >nul 2>&1

:: Debug build
if /i "%BUILD_DEBUG%"=="y" (
    mkdir build_debug
    cd build_debug
    cmake .. ^
        -DCMAKE_BUILD_TYPE=Debug ^
        -DCMAKE_PREFIX_PATH="%QT_PATH%" ^
        -DPYTHON_EXECUTABLE="%PYTHON_PATH%" ^
        -Dpybind11_DIR="%PYBIND11_DIR%" ^
        -DCMAKE_INSTALL_PREFIX="C:\Program Files"
    if %ERRORLEVEL% neq 0 (
        echo %ERROR_PREFIX% CMake configuration (Debug) failed
        exit /b 1
    )

    cmake --build . --config Debug -j %NUM_CORES%
    if %ERRORLEVEL% neq 0 (
        echo %ERROR_PREFIX% Build (Debug) failed
        exit /b 1
    )
    cd ..
)

:: Install Debug
if /i "%INSTALL_DEBUG%"=="y" (
    cd build_debug
    cmake --install . --config Debug
    if %ERRORLEVEL% neq 0 (
        echo %ERROR_PREFIX% Install (Debug) failed
        exit /b 1
    )
    cd ..
)

:: Release build
if /i "%BUILD_RELEASE%"=="y" (
    mkdir build_release
    cd build_release
    cmake .. ^
        -DCMAKE_BUILD_TYPE=Release ^
        -DCMAKE_PREFIX_PATH="%QT_PATH%" ^
        -DPYTHON_EXECUTABLE="%PYTHON_PATH%" ^
        -Dpybind11_DIR="%PYBIND11_DIR%" ^
        -DCMAKE_INSTALL_PREFIX="C:\Program Files"
    if %ERRORLEVEL% neq 0 (
        echo %ERROR_PREFIX% CMake configuration (Release) failed
        exit /b 1
    )

    cmake --build . --config Release -j %NUM_CORES%
    if %ERRORLEVEL% neq 0 (
        echo %ERROR_PREFIX% Build (Release) failed
        exit /b 1
    )
    cd ..
)

:: Install Release
if /i "%INSTALL_RELEASE%"=="y" (
    cd build_release
    cmake --install . --config Release
    if %ERRORLEVEL% neq 0 (
        echo %ERROR_PREFIX% Install (Release) failed
        exit /b 1
    )
    cd ..
)

:: Build Python wheel
if /i "%BUILD_WHL%"=="y" (
    echo Building Python wheel file...
    "%PYTHON_PATH%" setup.py build_ext --inplace
    "%PYTHON_PATH%" setup.py bdist_wheel
    if %ERRORLEVEL% neq 0 (
        echo %ERROR_PREFIX% Python wheel build failed
        exit /b 1
    )
)

:: Install Python wheel
if /i "%INSTALL_WHL%"=="y" (
    echo Installing Python wheel file to the current Python environment...
    "%PYTHON_PATH%" -m pip install dist\containerpy*.whl --force-reinstall
    if %ERRORLEVEL% neq 0 (
        echo %ERROR_PREFIX% Python wheel install failed
        exit /b 1
    )
)

echo Build and installation process completed successfully.
endlocal