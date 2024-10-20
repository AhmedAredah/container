cmake_minimum_required(VERSION 3.24)

set(PYTHON_LIB_NAME "${Container_NAME}Py")

# Ensure that the build type is Release unless explicitly overridden
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")


# Find Python using the specified Python executable
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Concurrent Xml Network Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Concurrent Xml Network Sql)

# Find pybind11 using HINTS
find_package(pybind11 REQUIRED CONFIG HINTS ${PYBIND11_HINTS_PATH})


set(BINDING_FILES
    bindcontainer.cpp
    containerext.cpp
    packageext.cpp
    containermapext.cpp
)

# Create a Python binding module
pybind11_add_module(${PYTHON_LIB_NAME} MODULE ${BINDING_FILES})

target_sources(${PYTHON_LIB_NAME}
PRIVATE
    containerext.h
#    containerext.cpp
    packageext.h
#    packageext.cpp
    containermapext.h
#    containermapext.cpp
)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Add the include directory where 'container.h' is located
target_include_directories(${PYTHON_LIB_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src/containerlib
)

# Link against the library target from the sibling project
target_link_libraries(${PYTHON_LIB_NAME} PRIVATE Container)

# Link against the required Qt libraries
target_link_libraries(${PYTHON_LIB_NAME} PRIVATE Qt6::Core Qt6::Concurrent Qt6::Network Qt6::Xml Qt6::Sql)

# Use the Python::Python target from find_package
target_link_libraries(${PYTHON_LIB_NAME} PRIVATE Python::Python)

# Ensure Container project is built first
add_dependencies(${PYTHON_LIB_NAME} ${Container_NAME})


# -----------------------------------------------
# Custom command to copy unitsLib.dll and Container.dll
# -----------------------------------------------

add_custom_command(TARGET ${PYTHON_LIB_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Container> $<TARGET_FILE_DIR:${PYTHON_LIB_NAME}>
)

# -----------------------------------------------
# Copy .pyd and .dll files to Python site-packages
# -----------------------------------------------
# Extract the path to the site-packages directory from the Python executable
execute_process(COMMAND ${Python_EXECUTABLE} -c "import sysconfig; print(sysconfig.get_paths()['purelib'])"
    OUTPUT_VARIABLE PYTHON_SITE_PACKAGES
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Ensure the container directory exists in site-packages
file(MAKE_DIRECTORY ${PYTHON_SITE_PACKAGES}/${PYTHON_LIB_NAME})

# -----------------------------------------------
# Automatically determine Qt's bin directory from QT_DIR using cmake_path()
# -----------------------------------------------
if(NOT DEFINED QT_DIR)
    message(FATAL_ERROR "QT_DIR is not defined. Please set the QT_DIR variable.")
endif()

# Go three levels up from QT_DIR to get the Qt installation directory and append /bin to it
cmake_path(GET QT_DIR PARENT_PATH QT_INSTALL_PREFIX_1)  # Goes from .../cmake/Qt6 to .../cmake
cmake_path(GET QT_INSTALL_PREFIX_1 PARENT_PATH QT_INSTALL_PREFIX_2)  # Goes from .../cmake to .../lib
cmake_path(GET QT_INSTALL_PREFIX_2 PARENT_PATH QT_INSTALL_PREFIX)  # Goes from .../lib to .../msvc2019_64

if(WIN32)
    # Windows platform
    # Set the Qt bin directory
    set(QT_BIN_DIR "${QT_INSTALL_PREFIX}/bin")
    set(QT_LIB_EXT ".dll")
    set(QT_LIB_PREF "")
elseif(APPLE)
    # macOS platform
    # Set the Qt bin directory
    set(QT_BIN_DIR "${QT_INSTALL_PREFIX}/lib")
    set(QT_LIB_EXT ".dylib")
    set(QT_LIB_PREF "")
else()
    # Linux and other platforms
    # Set the Qt bin directory
    set(QT_BIN_DIR "${QT_INSTALL_PREFIX}/lib")
    set(QT_LIB_EXT ".so.6")
    set(QT_LIB_PREF "lib")
endif()

message(STATUS "Qt bin directory: ${QT_BIN_DIR}")

# Full path to the setup.py script
set(SETUP_PY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/setup.py")

# Copy the .pyd and .dll files to the Python environment's site-packages directory
add_custom_command(TARGET ${PYTHON_LIB_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:${PYTHON_LIB_NAME}> ${PYTHON_SITE_PACKAGES}/${PYTHON_LIB_NAME}/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:${Container_NAME}> ${PYTHON_SITE_PACKAGES}/${PYTHON_LIB_NAME}/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${QT_BIN_DIR}/${QT_LIB_PREF}Qt6Core${QT_LIB_EXT} ${PYTHON_SITE_PACKAGES}/${PYTHON_LIB_NAME}/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${QT_BIN_DIR}/${QT_LIB_PREF}Qt6Network${QT_LIB_EXT} ${PYTHON_SITE_PACKAGES}/${PYTHON_LIB_NAME}/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${QT_BIN_DIR}/${QT_LIB_PREF}Qt6Xml${QT_LIB_EXT} ${PYTHON_SITE_PACKAGES}/${PYTHON_LIB_NAME}/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${QT_BIN_DIR}/${QT_LIB_PREF}Qt6Sql${QT_LIB_EXT} ${PYTHON_SITE_PACKAGES}/${PYTHON_LIB_NAME}/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/ContainerPy.pyi ${PYTHON_SITE_PACKAGES}/${PYTHON_LIB_NAME}/
)

# Pass the site-packages directory as an environment variable to the setup.py
add_custom_target(
    install_python_module ALL
    COMMAND ${Python_EXECUTABLE} ${SETUP_PY_PATH} install --prefix=${PYTHON_SITE_PACKAGES}
    COMMENT "Installing Python module using setup.py into ${PYTHON_SITE_PACKAGES}"
)

# Ensure install_python_module runs after the main build target
add_dependencies(install_python_module ${PYTHON_LIB_NAME})
