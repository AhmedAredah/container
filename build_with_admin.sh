#!/bin/bash

# Function to check command success
check_command() {
    if [ $? -ne 0 ]; then
        echo "Error: $1 failed"
        exit 1
    fi
}

# Change to the directory where the script is located
cd "$(dirname "$0")"

# Ensure Conda is available
if ! command -v conda &> /dev/null; then
    echo "Error: Conda is not installed or not in PATH."
    exit 1
fi

# Ensure an active Conda environment
if [[ -z "$CONDA_PREFIX" ]]; then
    echo "Error: No active Conda environment found. Please activate an Anaconda environment before running the script."
    exit 1
fi

ACTIVE_ENV=$CONDA_PREFIX
echo "Using Conda environment: $ACTIVE_ENV"

# Check Python path
PYTHON_PATH=$(which python)
if [[ -z "$PYTHON_PATH" ]]; then
    echo "Error: Python not found in the active Conda environment."
    exit 1
fi

echo "Using Python: $PYTHON_PATH"

# Ensure pip is up to date
$PYTHON_PATH -m pip install --no-cache-dir --upgrade pip
check_command "Pip upgrade"

# Install dependencies using pip
echo "Installing PyQt6 and pybind11..."
$PYTHON_PATH -m pip install --no-cache-dir pybind11 PyQt6
check_command "Pip dependencies installation"

# Detect platform
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    DEFAULT_QT_PATH="/home/ahmed/Qt/6.8.0/gcc_64/lib/cmake/Qt6"
    NUM_CORES=$(nproc)
elif [[ "$OSTYPE" == "darwin"* ]]; then
    DEFAULT_QT_PATH="/Users/ahmedaredah/Qt/6.8.0/macos/lib/cmake/Qt6"
    NUM_CORES=$(sysctl -n hw.ncpu)
else
    echo "Unsupported OS."
    exit 1
fi

# Ask user for Qt path
read -p "Use default Qt path ($DEFAULT_QT_PATH)? ([y]/n) " USE_DEFAULT_QT_PATH
USE_DEFAULT_QT_PATH=${USE_DEFAULT_QT_PATH:-y}

if [[ "$USE_DEFAULT_QT_PATH" == "y" ]]; then
    QT_PATH=$DEFAULT_QT_PATH
else
    read -p "Enter custom Qt path: " QT_PATH
    if [[ -z "$QT_PATH" ]]; then
        echo "Error: No Qt path provided. Exiting."
        exit 1
    fi
fi
echo "Using Qt path: $QT_PATH"

# Get pybind11 cmake directory
PYBIND11_DIR=$($PYTHON_PATH -c "import pybind11; print(pybind11.get_cmake_dir())")

# Ask the user about the build and installation steps
read -p "Do you want to build the Debug version? ([y]/n) " BUILD_DEBUG
BUILD_DEBUG=${BUILD_DEBUG:-y}

read -p "Do you want to build the Release version? ([y]/n) " BUILD_RELEASE
BUILD_RELEASE=${BUILD_RELEASE:-y}

read -p "Do you want to install the Debug build? ([y]/n) " INSTALL_DEBUG
INSTALL_DEBUG=${INSTALL_DEBUG:-y}

read -p "Do you want to install the Release build? ([y]/n) " INSTALL_RELEASE
INSTALL_RELEASE=${INSTALL_RELEASE:-y}

read -p "Do you want to build the Python wheel file? ([y]/n) " BUILD_WHL
BUILD_WHL=${BUILD_WHL:-y}

read -p "Do you want to install the Python wheel file? ([y]/n) " INSTALL_WHL
INSTALL_WHL=${INSTALL_WHL:-y}

# Only ask for docs and tests if building Release
if [[ "$BUILD_RELEASE" == "y" ]]; then
    read -p "Do you want to generate documentation? ([y]/n) " GENERATE_DOCS
    GENERATE_DOCS=${GENERATE_DOCS:-y}
    read -p "Do you want to run tests? ([y]/n) " RUN_TESTS
    RUN_TESTS=${RUN_TESTS:-y}
else
    GENERATE_DOCS="n"
    RUN_TESTS="n"
fi

# Display summary before proceeding
echo -e "\n**Summary of Selected Options:**"
echo "--------------------------------"
echo "Using Conda environment: $ACTIVE_ENV"
echo "Using Python: $PYTHON_PATH"
echo "Using Qt path: $QT_PATH"
echo "Build Debug version: $BUILD_DEBUG"
echo "Build Release version: $BUILD_RELEASE"
echo "Install Debug build: $INSTALL_DEBUG"
echo "Install Release build: $INSTALL_RELEASE"
echo "Build Python wheel file: $BUILD_WHL"
echo "Install Python wheel file: $INSTALL_WHL"
[[ "$BUILD_RELEASE" == "y" ]] && echo "Generate documentation: $GENERATE_DOCS"
[[ "$BUILD_RELEASE" == "y" ]] && echo "Run tests: $RUN_TESTS"
echo "--------------------------------"

# Ask if the user wants to modify any selection
read -p "Do you want to modify any selection? (y/[n]) " MODIFY_SELECTION
MODIFY_SELECTION=${MODIFY_SELECTION:-n}

if [[ "$MODIFY_SELECTION" == "y" ]]; then
    echo -e "\nRe-enter the options you want to change. Press Enter to keep the current selection."

    read -p "Build Debug version? (Current: $BUILD_DEBUG) ([y]/n) " NEW_BUILD_DEBUG
    BUILD_DEBUG=${NEW_BUILD_DEBUG:-$BUILD_DEBUG}

    read -p "Build Release version? (Current: $BUILD_RELEASE) ([y]/n) " NEW_BUILD_RELEASE
    BUILD_RELEASE=${NEW_BUILD_RELEASE:-$BUILD_RELEASE}

    read -p "Install Debug build? (Current: $INSTALL_DEBUG) ([y]/n) " NEW_INSTALL_DEBUG
    INSTALL_DEBUG=${NEW_INSTALL_DEBUG:-$INSTALL_DEBUG}

    read -p "Install Release build? (Current: $INSTALL_RELEASE) ([y]/n) " NEW_INSTALL_RELEASE
    INSTALL_RELEASE=${NEW_INSTALL_RELEASE:-$INSTALL_RELEASE}

    read -p "Build Python wheel file? (Current: $BUILD_WHL) ([y]/n) " NEW_BUILD_WHL
    BUILD_WHL=${NEW_BUILD_WHL:-$BUILD_WHL}

    read -p "Install Python wheel file? (Current: $INSTALL_WHL) ([y]/n) " NEW_INSTALL_WHL
    INSTALL_WHL=${NEW_INSTALL_WHL:-$INSTALL_WHL}

    if [[ "$BUILD_RELEASE" == "y" ]]; then
        read -p "Generate documentation? (Current: $GENERATE_DOCS) ([y]/n) " NEW_GENERATE_DOCS
        GENERATE_DOCS=${NEW_GENERATE_DOCS:-$GENERATE_DOCS}
        read -p "Run tests? (Current: $RUN_TESTS) ([y]/n) " NEW_RUN_TESTS
        RUN_TESTS=${NEW_RUN_TESTS:-$RUN_TESTS}
    fi
fi

echo -e "\nProceeding with the selected options..."

# Remove old build directories
echo "Removing old build directories..."
sudo rm -rf build_debug build_release dist *.egg-info

# Debug build
if [[ "$BUILD_DEBUG" == "y" ]]; then
    mkdir -p build_debug
    cd build_debug
    cmake .. \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_PREFIX_PATH="$QT_PATH" \
        -DPYTHON_EXECUTABLE="$PYTHON_PATH" \
        -Dpybind11_DIR="$PYBIND11_DIR" \
        -DCMAKE_INSTALL_PREFIX=/usr/local
    check_command "CMake configuration (Debug)"

    make -j$NUM_CORES
    check_command "Make (Debug)"
    cd ..
fi

# Install Debug
if [[ "$INSTALL_DEBUG" == "y" ]]; then
    cd build_debug
    sudo make install
    check_command "Make install (Debug)"
    cd ..
fi

# Release build
if [[ "$BUILD_RELEASE" == "y" ]]; then
    mkdir -p build_release
    cd build_release
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_PREFIX_PATH="$QT_PATH" \
        -DPYTHON_EXECUTABLE="$PYTHON_PATH" \
        -Dpybind11_DIR="$PYBIND11_DIR" \
        -DCMAKE_INSTALL_PREFIX=/usr/local \
        -DBUILD_DOCS="$([ "$GENERATE_DOCS" == "y" ] && echo "ON" || echo "OFF")" \
        -DBUILD_TESTING="$([ "$RUN_TESTS" == "y" ] && echo "ON" || echo "OFF")"
    check_command "CMake configuration (Release)"

    make -j$NUM_CORES
    check_command "Make (Release)"
    cd ..
fi

# Install Release
if [[ "$INSTALL_RELEASE" == "y" ]]; then
    cd build_release
    sudo make install
    check_command "Make install (Release)"
    cd ..
fi

# Build Python wheel
if [[ "$BUILD_WHL" == "y" ]]; then
    echo "Building Python wheel file..."
    $PYTHON_PATH setup.py build_ext --inplace
    $PYTHON_PATH setup.py bdist_wheel
    check_command "Python wheel build"
fi

# Install Python wheel
if [[ "$INSTALL_WHL" == "y" ]]; then
    echo "Installing Python wheel file to the current Python environment..."
    sudo chown -R $(whoami) $CONDA_PREFIX
    $PYTHON_PATH -m pip install dist/containerpy*.whl --force-reinstall
    check_command "Python wheel install"
fi

echo "Build and installation process completed successfully."
