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

# Activate Conda environment
eval "$(conda shell.bash hook)"
conda activate container_env || { echo "Error: Conda environment 'container_env' not found."; exit 1; }

# Check Python path
PYTHON_PATH=$(which python)
if [[ -z "$PYTHON_PATH" ]]; then
    echo "Error: Python not found in Conda environment."
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
    QT_PATH="/home/ahmed/Qt/6.8.0/gcc_64/lib/cmake/Qt6"
    NUM_CORES=$(nproc)
elif [[ "$OSTYPE" == "darwin"* ]]; then
    QT_PATH="/Users/ahmedaredah/Qt/6.8.0/macos/lib/cmake/Qt6"
    NUM_CORES=$(sysctl -n hw.ncpu)
else
    echo "Unsupported OS."
    exit 1
fi

# Get pybind11 cmake directory
PYBIND11_DIR=$($PYTHON_PATH -c "import pybind11; print(pybind11.get_cmake_dir())")

# Remove old build directories with proper permissions
echo "Removing old build directories..."
sudo rm -rf build_debug build_release dist *.egg-info

# Create Debug and Release build directories
mkdir build_debug build_release

# Debug build
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

sudo make install
check_command "Make install (Debug)"

cd ..

# Release build
cd build_release
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH="$QT_PATH" \
    -DPYTHON_EXECUTABLE="$PYTHON_PATH" \
    -Dpybind11_DIR="$PYBIND11_DIR" \
    -DCMAKE_INSTALL_PREFIX=/usr/local
check_command "CMake configuration (Release)"

make -j$NUM_CORES
check_command "Make (Release)"

sudo make install
check_command "Make install (Release)"

cd ..

# Build Python package
echo "Building Python package..."
$PYTHON_PATH setup.py build_ext --inplace
check_command "Python package build"


# # Install `conda-build` if not installed
# conda install -y conda-build
# check_command "Conda-build installation"

# # Change Conda build root directory to user space (to avoid permission issues)
# conda config --set croot ~/conda-bld
# mkdir -p ~/conda-bld
# chmod -R 777 ~/conda-bld  # Ensure it is writable

# # Remove any previous failed builds
# rm -rf ~/conda-bld/*

# # **Ensure `pybind11` is available in the Conda build environment**
# echo "Ensuring pybind11 is available in the Conda build environment..."
# CONDA_BUILD_ENV=$(mktemp -d)  # Temporary build environment
# conda create -y -p "$CONDA_BUILD_ENV" python=3.12 pybind11 pybind11-global
# check_command "Pybind11 installation inside Conda build environment"

# # Use this environment for Conda build
# conda build --no-build-id --no-test --croot "$CONDA_BUILD_ENV" conda/
# check_command "Conda package build"

# # Get package output path
# CONDA_PKG=$(conda build --no-build-id --no-test --croot "$CONDA_BUILD_ENV" conda/ --output | tail -n 1)

# # Install the conda package in cargonetsim environment
# conda install -y --use-local -n cargonetsim "$CONDA_PKG"
# check_command "Conda package installation"

echo "Build and installation process completed successfully."
