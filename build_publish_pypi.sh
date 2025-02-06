#!/bin/bash

set -e  # Exit on error

check_command() {
   if [ $? -ne 0 ]; then
       echo "❌ Error: $1 failed"
       exit 1
   fi
}

echo "🔄 Removing old build directories..."
rm -rf build dist *.egg-info
cp ./LICENSE python/LICENSE
cp ./README.md python/README.md

echo "🚀 Creating build directory..."
mkdir -p build

# Activate Conda
echo "🔧 Activating Conda environment..."
source ~/miniconda3/etc/profile.d/conda.sh
conda activate py38_env
PYTHON_PATH=$(which python)

# Install dependencies
echo "📦 Installing/upgrading required Python packages..."
$PYTHON_PATH -m pip install --upgrade pip setuptools wheel twine build pybind11 PyQt6 ninja cmake
check_command "Dependency installation"

# Check pip
echo "📦 Checking if pip is installed..."
$PYTHON_PATH -m ensurepip --default-pip
check_command "Ensure pip"

# Build package
echo "🛠️ Building Python package..."
$PYTHON_PATH -m build --no-isolation
check_command "Python package build"

echo "📂 Built package files:"
ls -lh dist/

# # Upload to PyPI
# echo "🌍 Uploading package to PyPI..."
# twine upload dist/*
# check_command "Twine upload"

echo "✅ Build process completed successfully!"