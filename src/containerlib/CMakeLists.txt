# Set minimum required CMake version
cmake_minimum_required(VERSION 3.24)

# Define library name from parent project
set(LIB_NAME ${Container_NAME})

# Find required Qt packages
find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Sql)

set(HEADERS
    ../../include//containerLib/package.h
    ../../include//containerLib/container.h
    ../../include//containerLib/containermap.h
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

# Set target properties
set_target_properties(${LIB_NAME} PROPERTIES
    CXX_VISIBILITY_PRESET default
    VISIBILITY_INLINES_HIDDEN ON
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    DEBUG_POSTFIX "d"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release
    EXPORT_NAME ${LIB_NAME}

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
# target_compile_definitions(${LIB_NAME}
#     PRIVATE
#         CONTAINER_LIBRARY
#         QT_NO_CAST_FROM_ASCII
#         QT_NO_CAST_TO_ASCII
# )

# Ensure symbols are correctly exported
target_compile_definitions(${LIB_NAME}
    PRIVATE
        CONTAINER_LIBRARY  # Defines CONTAINER_LIBRARY for internal use
    PUBLIC
        $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:CONTAINER_STATIC_DEFINE>
)

# Ensure symbols are exported correctly
set_target_properties(${LIB_NAME} PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
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
