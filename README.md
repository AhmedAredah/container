# Container Management Library

**Version**: 0.1.0

**Vendor**: (C) 2022-2025 Virginia Tech Transportation Institute - Center for Sustainable Mobility

**Authors**: Ahmed Aredah, Hesham Rakha

## Overview

The Container project is a comprehensive container management library with robust C++ and Python interfaces, designed for tracking, managing, and analyzing container logistics. The library provides flexible container operations, custom variable tracking, and supports both in-memory and database-backed storage.

## Project Structure

```
Container/
│
├── src/
│   ├── containerlib/          # C++ core library
│   └── containerpy/           # Python bindings
│
├── include/                   # Header files
├── tests/                     # Test suite
├── python/                    # Python package configuration
└── cmake/                     # CMake configuration files
```

## Prerequisites

### System Requirements
- Operating System: Windows, macOS, or Linux
- Compiler: GCC, Clang, or MSVC with C++23 support
- Build Tools: CMake (>= 3.24), Ninja (optional)

### Dependencies
- Qt 6.0 or higher
  - Download from: **https://www.qt.io/download-dev**
- Python 3.8+
- pybind11 (>= 2.10.0)

## Installation

### 1. Clone the Repository

```bash
git clone https://github.com/AhmedAredah/container.git
cd container
```

### 2. Install Dependencies

#### Qt Installation
1. Visit **https://www.qt.io/download-dev**
2. Download Qt 6.x Online Installer
3. Install Qt with the following components:
   - Qt Core
   - Qt SQL
   - Development Tools

#### Python Dependencies
It's recommended to use a Conda environment:

```bash
# Create and activate a Conda environment
conda create -n containerenv python=3.10
conda activate containerenv

# Install required dependencies
conda install -c conda-forge pybind11 cmake ninja setuptools pyqt
```

**Note**: 
- Replace `3.10` with your preferred Python version (3.8+)
- The environment uses Conda-forge channel to ensure compatible package versions
- This approach provides better dependency management, especially for C++ library bindings

### 3. Build the Project

#### Using Automated Build Script (Recommended)

The project includes a convenient `build_with_admin.sh` script that automates the build process:

```bash
# Make the script executable
chmod +x build_with_admin.sh

# Activate the anaconda env
conda activate containerenv

# Build, install, and create wheel package
./build_with_admin.sh
```

This script performs the following actions:
- Creates build directory
- Configures CMake
- Builds the library in debug/release
- Installs the library
- Generates wheel package
- Installs the Python package

#### Manual CMake Build

```bash
# Create build directory
mkdir build && cd build

# Configure the project
cmake ..

# Build the library
cmake --build .
```

#### Build Options
- `-DBUILD_SHARED_LIBS=ON/OFF`: Build shared or static libraries
- `-DBUILD_PYTHON_BINDINGS=ON/OFF`: Enable/disable Python bindings
- `-DBUILD_TESTING=ON/OFF`: Enable/disable tests
- `-DCMAKE_BUILD_TYPE=Debug/Release`: Set build configuration

### 4. Install the Library

```bash
# Install system-wide
cmake --build . --target install

# Or install in a specific directory
cmake -DCMAKE_INSTALL_PREFIX=/path/to/install ..
```

#### Using Wheel Package

After running the build script, you can install the wheel package:

```bash
# Install the generated wheel package
pip install dist/containerpy-*.whl
```

## Usage

### C++ Example

```cpp
#include <containerLib/container.h>
#include <containerLib/containermap.h>

int main() {
    ContainerCore::Container container("CNT001", ContainerCore::Container::twentyFT);
    container.setContainerCurrentLocation("Port A");
    container.addDestination("Port B");

    ContainerCore::ContainerMap containerMap;
    containerMap.addContainer("CNT001", &container);
}
```

### Python Example

```python
from containerpy import Container, ContainerMap, ContainerSize, HaulerType

# Create a container
container = Container("CNT001", ContainerSize.TwentyFT)
container.set_container_current_location("Port A")
set_container_next_destinations(["Port B", "Port C"])


# Add custom variables
container.add_custom_variable(HaulerType.Truck, "Weight", 1000)

# Create a container storage
container_map = ContainerMap()
container_map.add_container(container)

# Retrieve all containers in the storage
all_containers = container_map.get_all_containers()
```

## Testing

### Run C++ Tests
```bash
# After building
ctest
```

### Run Python Tests
```bash

# Activate the anaconda env
conda activate containerenv

# In the project root
python -m pytest tests/python
```

## PyPI Distribution

### Build whl file
```bash
# Build wheel package
python -m build

# Build Conda package
conda build conda/
```

## License

This project is licensed under the GNU Affero General Public License v3.0 (AGPL-3.0). 

See the [LICENSE](LICENSE) file for details.

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## Contact

- **Project Link**: https://github.com/AhmedAredah/container
- **Issue Tracker**: https://github.com/AhmedAredah/container/issues

**Note**: This is an early release. Contributions and feedback are welcome!