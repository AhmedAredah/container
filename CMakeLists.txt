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
# Set the default value for pybind11 hints path
set(PYBIND11_HINTS_PATH "C:/ProgramData/Anaconda3/envs/containerModeler/Lib/site-packages/pybind11/share/cmake/pybind11" CACHE STRING "Path to pybind11 CMake configuration")


# Add option to choose between shared and static libraries
option(BUILD_SHARED_LIBS "Build libraries as shared (ON) or static (OFF)" ON)
set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS} CACHE BOOL "Build libraries as shared (ON) or static (OFF)" FORCE)


# -------------------------------------------------------
# ---------------- RULES AND SUB PROJECTS ---------------
# -------------------------------------------------------

# include src directory
add_subdirectory(src)

# Installation rules
install(TARGETS ${Container_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
