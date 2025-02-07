@echo off
setlocal enabledelayedexpansion

:: Function to check command success
set "ERROR_PREFIX=Error:"

:: Change to the directory where the script is located
cd /d "%~dp0"

:: Ensure Conda is available
where conda >nul 2>&1
IF %ERRORLEVEL% neq 0 (
    echo %ERROR_PREFIX% Conda is not installed or not in PATH.
    exit /b 1
)

:: Ensure an active Conda environment
IF NOT DEFINED CONDA_PREFIX (
    echo %ERROR_PREFIX% No active Conda environment found. Please activate an Anaconda environment before running the script.
    exit /b 1
)

set "ACTIVE_ENV=%CONDA_PREFIX%"
echo Using Conda environment: %ACTIVE_ENV%

:: Ask the user for the Python executable path
:ask_python_path
set /p "PYTHON_PATH=Enter the full path to your Conda Python executable (e.g., C:\Users\Ahmed\.conda\envs\testing\python.exe): "
IF NOT EXIST "%PYTHON_PATH%" (
    echo %ERROR_PREFIX% The provided Python path does not exist. Please try again.
    goto ask_python_path
)

echo Using Python: %PYTHON_PATH%

:: Add CMake generator selection
echo Select CMake generator:
echo 1. Visual Studio 17 2022
echo 2. Visual Studio 16 2019 [default]
echo 3. Ninja
set /p "GENERATOR_CHOICE=Enter your choice (1-3): "
IF "%GENERATOR_CHOICE%"=="" set "GENERATOR_CHOICE=2"

IF "%GENERATOR_CHOICE%"=="1" (
    set "CMAKE_GENERATOR=Visual Studio 17 2022"
) else IF "%GENERATOR_CHOICE%"=="2" (
    set "CMAKE_GENERATOR=Visual Studio 16 2019"
) else IF "%GENERATOR_CHOICE%"=="3" (
    set "CMAKE_GENERATOR=Ninja"
) else (
    echo Invalid choice. Using Visual Studio 16 2019 as default.
    set "CMAKE_GENERATOR=Visual Studio 16 2019"
)

IF "%CMAKE_GENERATOR%"=="Visual Studio 17 2022" (
    IF exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat"
    ) else (
        echo %ERROR_PREFIX% Visual Studio 2022 Professional not found.
        exit /b 1
    )
) else IF "%CMAKE_GENERATOR%"=="Visual Studio 16 2019" (
    IF exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\Common7\Tools\VsDevCmd.bat" (
        call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\Common7\Tools\VsDevCmd.bat"
    ) else (
        echo %ERROR_PREFIX% Visual Studio 2019 Professional not found.
        exit /b 1
    )
)

:: Add architecture selection for Visual Studio generators
IF not "%CMAKE_GENERATOR%"=="Ninja" (
    echo Select architecture:
    echo 1. x64 [default]
    echo 2. Win32
    set /p "ARCH_CHOICE=Enter your choice (1-2): "
    IF "%ARCH_CHOICE%"=="" set "ARCH_CHOICE=1"

    IF "%ARCH_CHOICE%"=="1" (
        set "CMAKE_ARCHITECTURE=x64"
    ) else IF "%ARCH_CHOICE%"=="2" (
        set "CMAKE_ARCHITECTURE=Win32"
    ) else (
        echo Invalid choice. Using x64 as default.
        set "CMAKE_ARCHITECTURE=x64"
    )
) else (
    set "CMAKE_ARCHITECTURE="
)

echo Using CMake generator: %CMAKE_GENERATOR%
IF not "%CMAKE_ARCHITECTURE%"=="" echo Using architecture: %CMAKE_ARCHITECTURE%

:: Export generator and architecture for setup.py
set "CMAKE_GENERATOR=%CMAKE_GENERATOR%"
set "CMAKE_ARCHITECTURE=%CMAKE_ARCHITECTURE%"

:: Ensure pip is up to date
"%PYTHON_PATH%" -m pip install --no-cache-dir --upgrade pip
IF %ERRORLEVEL% neq 0 (
    echo %ERROR_PREFIX% Pip upgrade failed.
    exit /b 1
)

:: Install dependencies using pip
echo Installing PyQt6 and pybind11...
"%PYTHON_PATH%" -m pip install --no-cache-dir pybind11 PyQt6
IF %ERRORLEVEL% neq 0 (
    echo %ERROR_PREFIX% Pip dependencies installation failed.
    exit /b 1
)

:: Set default Qt path for Windows
set "DEFAULT_QT_PATH=C:\Qt\6.4.2\msvc2019_64\lib\cmake\Qt6"

:: Get number of CPU cores
:: Get number of CPU cores
for /f "tokens=2 delims==" %%a in ('wmic cpu get NumberOfCores /value') do (
    set "NUM_CORES=%%a"
)
if not defined NUM_CORES set "NUM_CORES=4"

:: Ask user for Qt path
set /p "USE_DEFAULT_QT_PATH=Use default Qt path (%DEFAULT_QT_PATH%)? ([y]/n) "
IF "%USE_DEFAULT_QT_PATH%"=="" set "USE_DEFAULT_QT_PATH=y"

IF /I "%USE_DEFAULT_QT_PATH%"=="y" (
    set "QT_CMAKE_PATH=%DEFAULT_QT_PATH%"
) else (
    set /p "QT_CMAKE_PATH=Enter custom Qt path: "
    IF "!QT_CMAKE_PATH!"=="" (
        echo %ERROR_PREFIX% No Qt path provided. Exiting.
        exit /b 1
    )
)
echo Using Qt path: %QT_CMAKE_PATH%

:: Set default Qt tools path for Windows
set "DEFAULT_QT_TOOLS_PATH=C:\Qt\Tools\CMake_64\bin"

:: Ask user for Qt tools path
set /p "USE_DEFAULT_QT_TOOLS_PATH=Use default Qt tools path (%DEFAULT_QT_TOOLS_PATH%)? ([y]/n) "
IF "%USE_DEFAULT_QT_TOOLS_PATH%"=="" set "USE_DEFAULT_QT_TOOLS_PATH=y"

IF /I "%USE_DEFAULT_QT_TOOLS_PATH%"=="y" (
    set "QT_TOOLS_PATH=%DEFAULT_QT_TOOLS_PATH%"
) else (
    set /p "QT_TOOLS_PATH=Enter custom Qt tools path: "
    IF "!QT_TOOLS_PATH!"=="" (
        echo %ERROR_PREFIX% No Qt tools path provided. Exiting.
        exit /b 1
    )
)
echo Using Qt tools path: %QT_TOOLS_PATH%

:: Get pybind11 cmake directory
for /f "tokens=*" %%i in ('cmd /c %PYTHON_PATH% -c "import pybind11; print(pybind11.get_cmake_dir())"') do set "PYBIND11_DIR=%%i"

:: Ask the user about the build and installation steps
set /p "BUILD_DEBUG=Do you want to build the Debug version? ([y]/n) "
IF "%BUILD_DEBUG%"=="" set "BUILD_DEBUG=y"

set /p "BUILD_RELEASE=Do you want to build the Release version? ([y]/n) "
IF "%BUILD_RELEASE%"=="" set "BUILD_RELEASE=y"

set /p "INSTALL_DEBUG=Do you want to install the Debug build? ([y]/n) "
IF "%INSTALL_DEBUG%"=="" set "INSTALL_DEBUG=y"

set /p "INSTALL_RELEASE=Do you want to install the Release build? ([y]/n) "
IF "%INSTALL_RELEASE%"=="" set "INSTALL_RELEASE=y"

set /p "BUILD_WHL=Do you want to build the Python wheel file? ([y]/n) "
IF "%BUILD_WHL%"=="" set "BUILD_WHL=y"

set /p "INSTALL_WHL=Do you want to install the Python wheel file? ([y]/n) "
IF "%INSTALL_WHL%"=="" set "INSTALL_WHL=y"

:: Display summary
echo.
echo **Summary of Selected Options:**
echo --------------------------------
echo Using Conda environment: %ACTIVE_ENV%
echo Using Python: %PYTHON_PATH%
echo Using Qt path: %QT_CMAKE_PATH%
echo Using CMake generator: %CMAKE_GENERATOR%
IF not "%CMAKE_ARCHITECTURE%"=="" echo Using architecture: %CMAKE_ARCHITECTURE%
echo Build Debug version: %BUILD_DEBUG%
echo Build Release version: %BUILD_RELEASE%
echo Install Debug build: %INSTALL_DEBUG%
echo Install Release build: %INSTALL_RELEASE%
echo Build Python wheel file: %BUILD_WHL%
echo Install Python wheel file: %INSTALL_WHL%
echo --------------------------------

:: Ask IF the user wants to modify any selection
set /p "MODIFY_SELECTION=Do you want to modify any selection? (y/[n]) "
IF "%MODIFY_SELECTION%"=="" set "MODIFY_SELECTION=n"

IF /I "%MODIFY_SELECTION%"=="y" (
    echo.
    echo Re-enter the options you want to change. Press Enter to keep the current selection.

    set /p "NEW_BUILD_DEBUG=Build Debug version? (Current: %BUILD_DEBUG%) ([y]/n) "
    IF not "%NEW_BUILD_DEBUG%"=="" set "BUILD_DEBUG=%NEW_BUILD_DEBUG%"

    set /p "NEW_BUILD_RELEASE=Build Release version? (Current: %BUILD_RELEASE%) ([y]/n) "
    IF not "%NEW_BUILD_RELEASE%"=="" set "BUILD_RELEASE=%NEW_BUILD_RELEASE%"

    set /p "NEW_INSTALL_DEBUG=Install Debug build? (Current: %INSTALL_DEBUG%) ([y]/n) "
    IF not "%NEW_INSTALL_DEBUG%"=="" set "INSTALL_DEBUG=%NEW_INSTALL_DEBUG%"

    set /p "NEW_INSTALL_RELEASE=Install Release build? (Current: %INSTALL_RELEASE%) ([y]/n) "
    IF not "%NEW_INSTALL_RELEASE%"=="" set "INSTALL_RELEASE=%NEW_INSTALL_RELEASE%"

    set /p "NEW_BUILD_WHL=Build Python wheel file? (Current: %BUILD_WHL%) ([y]/n) "
    IF not "%NEW_BUILD_WHL%"=="" set "BUILD_WHL=%NEW_BUILD_WHL%"

    set /p "NEW_INSTALL_WHL=Install Python wheel file? (Current: %INSTALL_WHL%) ([y]/n) "
    IF not "%NEW_INSTALL_WHL%"=="" set "INSTALL_WHL=%NEW_INSTALL_WHL%"
)

echo.
echo Proceeding with the selected options...

:: Remove old build directories
echo Removing old build directories...

:: Delete directories safely
for %%D in ("build_debug" "build_release" "dist") do (
    IF exist %%D (
        echo Deleting %%D...
        rmdir /s /q %%D 2>nul || echo !ERROR_PREFIX! Failed to delete %%D.
        timeout /t 2 >nul
    )
)

:: Delete all .egg-info files
for %%f in (*.egg-info) do (
    echo Deleting .egg-info files...
    del /a /f /q "*.egg-info" >nul 2>&1 || echo !ERROR_PREFIX! Failed to delete .egg-info files.
    timeout /t 2 >nul
)

:: Check and delete the dist directory
IF exist "dist" (
    echo Deleting dist...
    rmdir /s /q "dist"
    timeout /t 2 >nul
)

:: Check and delete .egg-info files safely
for %%f in (*.egg-info) do (
    echo Deleting %%f...
    del /f /q "%%f"
    timeout /t 2 >nul
)

:: Wait before proceeding
timeout /t 2 >nul
echo.

:: Build Debug
IF "%BUILD_DEBUG%"=="y" (
    echo Building Debug version...
    mkdir build_debug
    cd build_debug

    IF "%CMAKE_ARCHITECTURE%" NEQ "" (
        cmake .. -G "%CMAKE_GENERATOR%" -A "%CMAKE_ARCHITECTURE%" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="%QT_CMAKE_PATH%" -DPYTHON_EXECUTABLE="%PYTHON_PATH%" -Dpybind11_DIR="%PYBIND11_DIR%" -DCMAKE_INSTALL_PREFIX="C:\Program Files\ContainerLib"
    ) ELSE IF "%CMAKE_ARCHITECTURE%" == "" (
        cmake .. -G "%CMAKE_GENERATOR%" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="%QT_CMAKE_PATH%" -DPYTHON_EXECUTABLE="%PYTHON_PATH%" -Dpybind11_DIR="%PYBIND11_DIR%" -DCMAKE_INSTALL_PREFIX="C:\Program Files\ContainerLib"
    )

    timeout /t 2 >nul

    cmake --build . --config Debug -j %NUM_CORES%
    cd ..
)


:: Install Debug
IF /I "%INSTALL_DEBUG%"=="y" (
    IF /I "%BUILD_DEBUG%"=="y" (
        cd build_debug
        cmake --install . --config Debug
        cd ..
    )
)

:: Wait before proceeding
timeout /t 2 >nul

:: Build Release
IF /I "%BUILD_RELEASE%"=="y" (
    mkdir build_release
    cd build_release

    IF "%CMAKE_ARCHITECTURE%" NEQ "" (
        cmake .. -G "%CMAKE_GENERATOR%" -A "%CMAKE_ARCHITECTURE%" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="%QT_CMAKE_PATH%" -DPYTHON_EXECUTABLE="%PYTHON_PATH%" -Dpybind11_DIR="%PYBIND11_DIR%" -DCMAKE_INSTALL_PREFIX="C:\Program Files\ContainerLib"
    ) ELSE IF "%CMAKE_ARCHITECTURE%" == "" (
        cmake .. -G "%CMAKE_GENERATOR%" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="%QT_CMAKE_PATH%" -DPYTHON_EXECUTABLE="%PYTHON_PATH%" -Dpybind11_DIR="%PYBIND11_DIR%" -DCMAKE_INSTALL_PREFIX="C:\Program Files\ContainerLib"
    )

    timeout /t 2 >nul

    cmake --build . --config Release -j %NUM_CORES%
    cd ..
)


:: Wait before proceeding
timeout /t 2 >nul

:: Install Release
IF /I "%INSTALL_RELEASE%"=="y" (
    IF /I "%BUILD_RELEASE%"=="y" (
        cd build_release
        cmake --install . --config Release
        cd ..
    )
)

:: Wait before proceeding
timeout /t 2 >nul

:: Build Python Wheel
IF /I "%BUILD_WHL%"=="y" (
    echo.
    echo Building Python wheel file in %cd% ...
    "%PYTHON_PATH%" setup.py build_ext --inplace
    "%PYTHON_PATH%" setup.py bdist_wheel --plat-name=win_amd64

    echo Contents of dist directory:
    dir dist
)

:: Install Python Wheel
IF /I "%INSTALL_WHL%"=="y" (
    echo Installing Python wheel file...
    IF exist "dist\*.whl" (
        for %%F in (dist\*.whl) do (
            echo Installing wheel: %%F
            "%PYTHON_PATH%" -m pip install "%%F" --force-reinstall
            IF !ERRORLEVEL! neq 0 (
                echo %ERROR_PREFIX% Failed to install wheel file
                exit /b 1
            )
        )
    ) else (
        echo %ERROR_PREFIX% No wheel file found in dist directory
        exit /b 1
    )
)

echo Build and installation process completed successfully.
endlocal