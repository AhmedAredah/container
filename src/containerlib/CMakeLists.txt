# Set minimum required CMake version
cmake_minimum_required(VERSION 3.24)

# Define library name from parent project
set(LIB_NAME ${Container_NAME})

# Find required Qt packages
find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Sql)

set(HEADERS
    ../../include/containerLib/package.h
    ../../include/containerLib/container.h
    ../../include/containerLib/containermap.h
)

# Ensure that MOC is enabled for Qt classes
qt6_wrap_cpp(MOC_SOURCES ${HEADERS})

# Define source files with proper paths
set(SOURCES
    package.cpp
    container.cpp
    containermap.cpp
)

set(SOURCES ${SOURCES} ${MOC_SOURCES})

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

# Set target properties with multi-config considerations
set_target_properties(${LIB_NAME} PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    DEBUG_POSTFIX "d"
    EXPORT_NAME ${LIB_NAME}
)

# Handle multi-config generators properly
get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(IS_MULTI_CONFIG)
    # For multi-config generators, set per-config output directories
    set_target_properties(${LIB_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/bin
        LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib
        LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib
        LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/lib
        LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/lib
    )
    
    # Additional postfixes for other configurations
    set_target_properties(${LIB_NAME} PROPERTIES
        RELWITHDEBINFO_POSTFIX ""
        MINSIZEREL_POSTFIX ""
    )
else()
    # For single-config generators, use the standard directories
    set_target_properties(${LIB_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    )
endif()

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

# Ensure symbols are correctly exported
target_compile_definitions(${LIB_NAME}
    PRIVATE
        CONTAINER_LIBRARY  # Defines CONTAINER_LIBRARY for internal use
    PUBLIC
        $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:CONTAINER_STATIC_DEFINE>
)

# Add compiler warnings
if(MSVC)
    target_compile_options(${LIB_NAME} PRIVATE /W4)
else()
    target_compile_options(${LIB_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Optional: Add debugging information
target_compile_definitions(${LIB_NAME} PRIVATE
    $<$<CONFIG:Debug>:DEBUG_BUILD>
)

# Installation configuration
include(GNUInstallDirs)

# Install the library with proper multi-config support
install(TARGETS ${LIB_NAME}
    EXPORT ContainerTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT Runtime
        NAMELINK_COMPONENT Development
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT Development
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT Runtime
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Print configuration summary
message(STATUS "")
message(STATUS "${LIB_NAME} Library Configuration")
message(STATUS "--------------------------------")
message(STATUS "Version:          ${PROJECT_VERSION}")
message(STATUS "Install prefix:   ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Library type:     ${BUILD_SHARED_LIBS}")
message(STATUS "Build type:       ${CMAKE_BUILD_TYPE}")
message(STATUS "Qt version:       ${QT_VERSION_MAJOR}")
get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(IS_MULTI_CONFIG)
    message(STATUS "Generator:        Multi-config (${CMAKE_GENERATOR})")
else()
    message(STATUS "Generator:        Single-config (${CMAKE_GENERATOR})")
endif()
message(STATUS "")