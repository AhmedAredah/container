# min required cmake version
cmake_minimum_required(VERSION 3.24)

# Define the project name (Container) and
# the programming language used (CXX for C++)
set(Container_VERSION "0.0.1" CACHE STRING "Project version" FORCE)
set(Container_NAME "Container" CACHE STRING "Project name" FORCE)
set(Container_VENDOR "(C) 2022-2023 Virginia Tech Transportation Institute - Center for Sustainable Mobility." CACHE STRING "Project vendor" FORCE)
# Get the current date and time
string(TIMESTAMP BUILD_DATE "%Y-%m-%d %H:%M:%S")
# Set the BUILD_DATE variable
set(BUILD_DATE ${BUILD_DATE} CACHE STRING "Project build time" FORCE)

# Extract major, minor, and patch version from Container_VERSION
string(REPLACE "." ";" VERSION_LIST ${Container_VERSION})
list(GET VERSION_LIST 0 Container_VERSION_MAJOR)
list(GET VERSION_LIST 1 Container_VERSION_MINOR)
list(GET VERSION_LIST 2 Container_VERSION_PATCH)

project(${Container_NAME} VERSION ${Container_VERSION} LANGUAGES CXX)

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Compiler settings
# Set the C++20 standard to be used for compiling
set(CMAKE_CXX_STANDARD 23)
# Ensure that the selected C++ standard is a
# requirement for the compiler
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Enable Qt's Automatic User Interface Compiler (UIC)
set(CMAKE_AUTOUIC ON)
# Enable Qt's Meta-Object Compiler (MOC) which allows
# the use of Qt features such as signals and slots
set(CMAKE_AUTOMOC ON)
# Enable Qt's Resource Compiler (RCC) for compiling
# resource files into binary format
set(CMAKE_AUTORCC ON)


# Platform-specific compiler flags
if(MSVC)
    # MSVC-specific flags
    add_compile_options(/W4 /MP)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(/Od /Zi)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(/O2)
    endif()
else()
    # GCC/Clang-specific flags
    add_compile_options(-Wall)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-O0 -g)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3)
    endif()
endif()

# Add definitions based on build type
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif()

# -------------------------------------------------------
# ------------------ OPTIONAL PARAMATERS ----------------
# -------------------------------------------------------

# Add option to choose between building python package or not
option(BUILD_PYTHON_PACKAGE "Build the Python bindings for the project" ON)
set(BUILD_PYTHON_PACKAGE ${BUILD_PYTHON_PACKAGE} CACHE BOOL "Build the Python bindings for the project" FORCE)

# If Python bindings are to be built, set the necessary variables
if(BUILD_PYTHON_PACKAGE)

    # Set the default Python executable location to be the Conda environment at
    ## "C:\ProgramData\Anaconda3\envs\containerModeler\python.exe"
    # Allow the user to override this value with -DPYTHON_EXECUTABLE=<path>
    if(WIN32)
        # Windows path
        set(PYTHON_EXECUTABLE "C:/ProgramData/Anaconda3/envs/cargonetsim/python.exe" CACHE PATH "Path to the Python executable")
    elseif(APPLE)
        # macOS path
        set(PYTHON_EXECUTABLE "/opt/anaconda3/envs/cargonetsim/bin/python" CACHE PATH "Path to the Python executable")
    elseif(UNIX)
        # Linux path
        set(PYTHON_EXECUTABLE "/home/ahmed/miniconda3/envs/cargonetsim/bin/python" CACHE PATH "Path to the Python executable")
    else()
        message(FATAL_ERROR "Unsupported operating system")
    endif()

    # Set Python_EXECUTABLE variable to override the default one found by CMake
    set(Python_EXECUTABLE ${PYTHON_EXECUTABLE})

    # Make sure the user provides or confirms the Python path
    if(NOT EXISTS ${Python_EXECUTABLE})
        message(FATAL_ERROR "The specified Python executable does not exist: ${Python_EXECUTABLE}")
    else()
        message(STATUS "Using Python from: ${Python_EXECUTABLE}")
    endif()

    # Attempt to find the pybind11 CMake directory using the Python executable
    execute_process(
        COMMAND ${Python_EXECUTABLE} -c "import pybind11; print(pybind11.get_cmake_dir())"
        OUTPUT_VARIABLE PYBIND11_CMAKE_DIR
        ERROR_VARIABLE PYBIND11_ERROR
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # Check if the command succeeded (i.e., pybind11 is installed)
    if (PYBIND11_CMAKE_DIR)
        # Cache the PYBIND11_HINTS_PATH variable for later use
        set(PYBIND11_HINTS_PATH ${PYBIND11_CMAKE_DIR} CACHE STRING "Path to the pybind11 CMake configuration directory")
        message(STATUS "pybind11 is installed. CMake directory: ${PYBIND11_HINTS_PATH}")
    else()
        message(FATAL_ERROR "pybind11 is not installed or could not be found. Error: ${PYBIND11_ERROR}")
    endif()

    # Attempt to find if setuptools is installed using the Python executable
    execute_process(
        COMMAND ${Python_EXECUTABLE} -c "import setuptools; print(setuptools.__version__)"
        OUTPUT_VARIABLE SETUPTOOLS_VERSION
        ERROR_VARIABLE SETUPTOOLS_ERROR
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # Check if the command succeeded (i.e., setuptools is installed)
    if (SETUPTOOLS_VERSION)
        # Cache the setuptools version for later use
        set(SETUPTOOLS_INSTALLED TRUE CACHE BOOL "Indicates if setuptools is installed")
        message(STATUS "setuptools is installed. Version: ${SETUPTOOLS_VERSION}")
    else()
        message(FATAL_ERROR "setuptools is not installed or could not be found. Error: ${SETUPTOOLS_ERROR}")
    endif()

endif()

# -------------------------------------------------------
# ---------------- RULES AND SUB PROJECTS ---------------
# -------------------------------------------------------

# include src directory
add_subdirectory(src)

# Installation rules
install(TARGETS ${Container_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
