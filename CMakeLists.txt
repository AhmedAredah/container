# Minimum required CMake version
cmake_minimum_required(VERSION 3.24)

# Project definition with metadata
project(Container
    VERSION 0.1.0
    DESCRIPTION "Container management library with Python bindings"
    HOMEPAGE_URL "https://github.com/yourusername/container"
    LANGUAGES CXX
)

# Set the library name
set(Container_NAME Container)

# Set vendor info
set(Container_VENDOR "(C) 2022-2025 Virginia Tech Transportation Institute" CACHE STRING "Project vendor")
string(TIMESTAMP BUILD_DATE "%Y-%m-%d %H:%M:%S" UTC)
set(BUILD_DATE ${BUILD_DATE} CACHE STRING "Project build time" FORCE)

# Extract version components
set(Container_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(Container_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(Container_VERSION_PATCH ${PROJECT_VERSION_PATCH})

# Global options
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(BUILD_TESTING "Build tests" ON)
option(BUILD_DOCS "Build documentation" ON)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS 
        "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Global compiler settings
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# Enable Qt's UIC, MOC, RCC
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Platform-specific compiler flags
if(MSVC)
    add_compile_options(/W4 /MP)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(/Od /Zi)
    else()
        add_compile_options(/O2)
    endif()
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-O0 -g)
    else()
        add_compile_options(-O3)
    endif()
endif()

# Find Qt
find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Sql)

# Python bindings configuration
if(BUILD_PYTHON_BINDINGS)
    # Set default Python executable location based on platform
    if(WIN32)
        set(PYTHON_EXECUTABLE "C:/ProgramData/Anaconda3/envs/containerModeler/python.exe"
            CACHE PATH "Path to Python executable")
    elseif(APPLE)
        set(PYTHON_EXECUTABLE "/opt/anaconda3/envs/cargonetsim/bin/python"
            CACHE PATH "Path to Python executable")
    elseif(UNIX)
        set(PYTHON_EXECUTABLE "/home/ahmed/miniconda3/envs/cargonetsim/bin/python"
            CACHE PATH "Path to Python executable")
    endif()

    # Verify Python executable
    if(NOT EXISTS ${PYTHON_EXECUTABLE})
        message(FATAL_ERROR "Python executable not found at: ${PYTHON_EXECUTABLE}")
    endif()
    
    set(Python_EXECUTABLE ${PYTHON_EXECUTABLE})
    
    # Find pybind11
    execute_process(
        COMMAND ${Python_EXECUTABLE} -c "import pybind11; print(pybind11.get_cmake_dir())"
        OUTPUT_VARIABLE PYBIND11_CMAKE_DIR
        ERROR_VARIABLE PYBIND11_ERROR
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    
    if(PYBIND11_CMAKE_DIR)
        set(PYBIND11_HINTS_PATH ${PYBIND11_CMAKE_DIR}
            CACHE STRING "Path to pybind11 CMake configuration")
    else()
        message(FATAL_ERROR "pybind11 not found: ${PYBIND11_ERROR}")
    endif()
endif()

# Add main library and bindings
add_subdirectory(src/containerlib)

if(BUILD_PYTHON_BINDINGS)
    add_subdirectory(src/containerpy)
endif()

# Testing configuration
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

# Documentation configuration
if(BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        add_subdirectory(docs)
    else()
        message(STATUS "Doxygen not found, documentation will not be built")
    endif()
endif()

# Installation configuration
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Generate package version file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/ContainerConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Generate package config file
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ContainerConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/ContainerConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Container
)

# Install config files
install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/ContainerConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/ContainerConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Container
)

# Install targets
install(
    EXPORT ContainerTargets
    FILE ContainerTargets.cmake
    NAMESPACE Container::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Container
)

# Install headers
install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

# Export package for use from build tree
export(EXPORT ContainerTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/ContainerTargets.cmake"
    NAMESPACE Container::
)

# Register package in user's package registry
export(PACKAGE Container)

# Print configuration summary
message(STATUS "")
message(STATUS "Configuration Summary")
message(STATUS "--------------------")
message(STATUS "Version:              ${PROJECT_VERSION}")
message(STATUS "Build type:           ${CMAKE_BUILD_TYPE}")
message(STATUS "Shared libs:          ${BUILD_SHARED_LIBS}")
message(STATUS "Python bindings:      ${BUILD_PYTHON_BINDINGS}")
message(STATUS "Build testing:        ${BUILD_TESTING}")
message(STATUS "Build documentation:  ${BUILD_DOCS}")
message(STATUS "Install prefix:       ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
