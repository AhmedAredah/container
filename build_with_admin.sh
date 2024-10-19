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

# Set the Qt path
QT_PATH="/home/ahmed/Qt/6.4.3/gcc_64/lib/cmake/Qt6"

# Configure and build in Debug mode
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="$QT_PATH"
make install -j$(nproc)

# Configure and build in Release mode
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$QT_PATH"
make install -j$(nproc)

echo "Build process completed."
