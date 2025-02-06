# Set minimum required CMake version
cmake_minimum_required(VERSION 3.24)

# Define library name from parent project
set(LIB_NAME ${Container_NAME})

# Find required Qt packages
find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Sql)

# Define source files with proper paths
set(SOURCES
    package.cpp
    container.cpp
    containermap.cpp
)

# Create library target
add_library(${LIB_NAME} ${SOURCES})

# Add alias for use in other targets
add_library(${LIB_NAME}::${LIB_NAME} ALIAS ${LIB_NAME})

# Generate export header
include(GenerateExportHeader)
generate_export_header(${LIB_NAME}
    BASE_NAME ${LIB_NAME}
    EXPORT_MACRO_NAME ${LIB_NAME}_EXPORT
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/include/${LIB_NAME}_export.h
    DEPRECATED_MACRO_NAME ${LIB_NAME}_DEPRECATED
    NO_EXPORT_MACRO_NAME ${LIB_NAME}_NO_EXPORT
    STATIC_DEFINE ${LIB_NAME}_STATIC_DEFINE
    NO_DEPRECATED_MACRO_NAME ${LIB_NAME}_NO_DEPRECATED
    DEFINE_NO_DEPRECATED
)

# Set target properties
set_target_properties(${LIB_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    DEBUG_POSTFIX "d"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release
    EXPORT_NAME ${LIB_NAME}
)

set_target_properties(${LIB_NAME} PROPERTIES
    CXX_VISIBILITY_PRESET default
    VISIBILITY_INLINES_HIDDEN OFF
)

# Set include directories
target_include_directories(${LIB_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Link Qt libraries
target_link_libraries(${LIB_NAME}
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Sql
)

# Set compile definitions
target_compile_definitions(${LIB_NAME}
    PRIVATE
        CONTAINER_LIBRARY
        QT_NO_CAST_FROM_ASCII
        QT_NO_CAST_TO_ASCII
)

# Installation configuration
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Install the library
install(TARGETS ${LIB_NAME}
    EXPORT ${LIB_NAME}Targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install header files
install(FILES
    ${CMAKE_SOURCE_DIR}/include/containerLib/Container_global.h
    ${CMAKE_SOURCE_DIR}/include/containerLib/package.h
    ${CMAKE_SOURCE_DIR}/include/containerLib/container.h
    ${CMAKE_SOURCE_DIR}/include/containerLib/containermap.h
    ${CMAKE_SOURCE_DIR}/include/containerLib/containercache.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/${LIB_NAME}_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${LIB_NAME}
)

# Create and install the config file
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIB_NAME}
    PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR
)

# Create and install the version file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Install config files
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIB_NAME}
)

# Install targets file
install(EXPORT ${LIB_NAME}Targets
    FILE ${LIB_NAME}Targets.cmake
    NAMESPACE ${LIB_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIB_NAME}
)

# Add compiler warnings
if(MSVC)
    target_compile_options(${LIB_NAME} PRIVATE /W4)
else()
    target_compile_options(${LIB_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Optional: Add debugging information
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${LIB_NAME} PRIVATE DEBUG_BUILD)
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "${LIB_NAME} Configuration Summary")
message(STATUS "-----------------------------")
message(STATUS "Version:          ${PROJECT_VERSION}")
message(STATUS "Install prefix:   ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Library type:     ${BUILD_SHARED_LIBS}")
message(STATUS "Build type:       ${CMAKE_BUILD_TYPE}")
message(STATUS "Qt version:       ${QT_VERSION_MAJOR}")
message(STATUS "")
