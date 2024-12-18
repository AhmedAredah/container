#!/bin/bash

# Check if running with admin privileges
if [[ $EUID -ne 0 ]]; then
    echo "Requesting admin privileges..."
    sudo "$0" "$@"
    exit
fi

# Change to the directory where the script is located
cd "$(dirname "$0")"

# Remove existing build directory
rm -rf build

# Create build directory
mkdir build
cd build

# Set paths based on the operating system
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Linux paths
    QT_PATH="/home/ahmed/Qt/6.8.0/gcc_64/lib/cmake/Qt6"
    PYTHON_PATH="/home/ahmed/miniconda3/envs/cargonetsim/bin/python"
    NUM_CORES=$(nproc)
elif [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS paths
    QT_PATH="/Users/ahmedaredah/Qt/6.8.0/macos/lib/cmake/Qt6"
    PYTHON_PATH="/opt/anaconda3/envs/cargonetsim/bin/python"
    NUM_CORES=$(sysctl -n hw.ncpu)
else
    echo "Unsupported operating system."
    exit 1
fi

# Configure and build in Debug mode
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="$QT_PATH" -DPYTHON_EXECUTABLE="$PYTHON_PATH"
make install -j$NUM_CORES

# Configure and build in Release mode
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$QT_PATH" -DPYTHON_EXECUTABLE="$PYTHON_PATH"
make install -j$NUM_CORES

echo "Build process completed."
