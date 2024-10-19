# Container Project
**Version**: 0.0.1

**Vendor**: (C) 2022-2023 Virginia Tech Transportation Institute - Center for Sustainable Mobility

**Authors**: Ahmed Aredah, Hesham Rakha

The Container project consists of two main subprojects:

 - containerlib: A C++ library built using Qt 6.4.3.
 - containerpy: A Python binding for containerlib using pybind11, enabling Python applications to access the features provided by containerlib. It is recommended to build containerpy within a Conda environment for better dependency management.

## Prerequisites
### General Requirements
 - CMake >= 3.24
 - Qt 6.4.3 (or higher)
 - C++ Compiler supporting C++23 standard (e.g., GCC, Clang, MSVC)

### containerlib Specific
 - Qt Components:
   - Core
   - Sql
   
### containerpy Specific
 - Python (Recommended: Python 3.8+)
 - Conda environment
 - pybind11 (pip install pybind11)
 - setuptools (pip install setuptools)
 - Qt Components:
   - Core
   - Concurrent
   - Xml
   - Network
   - Sql

## Project Structure
```plaintext
Container/
│
├── src/
│   ├── containerlib/          # C++ library
│   │   ├── CMakeLists.txt     # Build configuration for containerlib
│   │   ├── container.h        # Header files
│   │   └── ...                # Other source files
│   │
│   └── containerpy/           # Python bindings
│       ├── CMakeLists.txt     # Build configuration for containerpy
│       ├── bindcontainer.cpp  # pybind11 binding source files
│       └── ...                # Other Python binding files
│
├── CMakeLists.txt             # Main build configuration
└── README.md                  # Project documentation
```

## Building the Project
### Step 1: Clone the Repository
```bash
git clone <repository-url>
cd Container
```
### Step 2: Configure the Project with CMake
Make sure CMake is available and correctly configured. Set up the build using the following command:

```bash
mkdir build && cd build
cmake ..
```
This will configure both containerlib and containerpy for building.

### Step 3: Building containerlib
containerlib is the C++ core library that must be built first:

 1. Run the following command to build:

```bash
cmake --build . --target Container
```

 2. Compiler Settings:
 - containerlib is compiled with Qt 6.4.3.
 - The library supports both Debug and Release configurations.

 3. Qt Dependencies: Make sure Qt is installed and the environment variable QT_DIR is set correctly. For example:

```bash
export QT_DIR=/path/to/qt6.4.3
```

### Step 4: Building containerpy
*containerpy* is the Python interface for containerlib. It should be built after containerlib.

 1. **Recommended: Use Conda Environment**

```bash
conda create -n containerpy python=3.10
conda activate containerpy
```

 2. **Ensure pybind11 and setuptools Are Installed**:

```bash
pip install pybind11 setuptools
```

 3. **Build containerpy**:

```bash
Copy code
cmake --build . --target ContainerPy
```
4. **Qt Dependencies for containerpy**: Ensure the same environment variable QT_DIR is set for correct linking of Qt components:

```bash
Copy code
export QT_DIR=/path/to/qt6.4.3
```

### Step 5: Installing containerpy
The Python bindings can be installed using CMake:

```bash
cmake --build . --target install_python_module
```
This will copy the containerpy shared library and necessary Qt dependencies to the Python site-packages directory.

## Using containerpy
After successful build and installation, containerpy can be used in Python scripts as follows:

```python
Copy code
import ContainerPy

# Example of using ContainerPy functionality
package = ContainerPy.Package('package1')
```

Ensure that the containerpy module is accessible by Python. If you encounter import errors, verify that it has been installed in your active Python environment's site-packages.

## Notes on Platform-Specific Builds
 - Windows: Ensure .dll files are copied correctly. Use Visual Studio to build the project.
 - macOS: Ensure .dylib files are correctly linked.
 - Linux: Use .so files and make sure all necessary dependencies are installed.

## License
(C) 2024-2025 Virginia Tech Transportation Institute - Center for Sustainable Mobility. All rights reserved.
