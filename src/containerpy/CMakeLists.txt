# Set minimum required CMake version
cmake_minimum_required(VERSION 3.24)

set(PYTHON_LIB_NAME "ContainerPy")

# Find packages
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Sql)
find_package(pybind11 REQUIRED CONFIG HINTS ${PYBIND11_HINTS_PATH})

# Debug info
get_filename_component(PYTHON_LIB_PATH "${Python_LIBRARIES}" DIRECTORY)
message(STATUS "Python Library Dirs: ${Python_LIBRARY_DIRS}")
message(STATUS "Python Libraries: ${Python_LIBRARIES}")
message(STATUS "Python Library Path: ${PYTHON_LIB_PATH}")

# Source files
set(BINDING_FILES
    bindcontainer.cpp
    containerext.cpp
    packageext.cpp
    containermapext.cpp
)

set(BINDING_HEADERS
    containerext.h
    packageext.h
    containermapext.h
)

# Create Python module
pybind11_add_module(${PYTHON_LIB_NAME} MODULE ${BINDING_FILES} ${BINDING_HEADERS})

# Properties
set_target_properties(${PYTHON_LIB_NAME} PROPERTIES
    OUTPUT_NAME "ContainerPy"
    DEBUG_POSTFIX ""
    POSITION_INDEPENDENT_CODE ON
)

# RPATH settings
if(UNIX)
    set_target_properties(${PYTHON_LIB_NAME} PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "$ORIGIN:${PYTHON_LIB_PATH}"  # Add Python library path
        SKIP_BUILD_RPATH FALSE
    )
endif()

# Include and link
target_include_directories(${PYTHON_LIB_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/containerlib
)

target_link_libraries(${PYTHON_LIB_NAME} PRIVATE
    Container
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Sql
)

# Link against Python if not on Apple
if(NOT APPLE)
    target_link_libraries(${PYTHON_LIB_NAME} PRIVATE Python::Python)
endif()



# Print configuration summary
message(STATUS "")
message(STATUS "Python Bindings Configuration")
message(STATUS "----------------------------")
message(STATUS "Python version:      ${Python_VERSION}")
message(STATUS "Python executable:   ${Python_EXECUTABLE}")
message(STATUS "pybind11 version:    ${pybind11_VERSION}")
message(STATUS "")
