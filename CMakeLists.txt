cmake_minimum_required(VERSION 3.14)

project(Container VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Sql)

add_library(Container SHARED
  src/Container_global.h
  src/package.h src/package.cpp
  src/container.h src/container.cpp
  src/containermap.h src/containermap.cpp
)


# Ensure that the library has a 'd' postfix in Debug builds
set_target_properties(Container PROPERTIES
  DEBUG_POSTFIX "d"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release
)

target_link_libraries(Container PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Sql
)

# Export the include directories for other projects to use
target_include_directories(Container
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include/Container>
)

target_compile_definitions(Container PRIVATE CONTAINER_LIBRARY)


# Set the install directories
install(TARGETS Container
    EXPORT ContainerTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(FILES
    src/container.h
    src/package.h
    src/Container_global.h
    src/containermap.h
    DESTINATION include/Container
)

# Generate and install the CMake configuration files
include(CMakePackageConfigHelpers)

# Generate the ConfigVersion.cmake file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/ContainerConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Create the Config.cmake file
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/ContainerConfig.cmake"
    INSTALL_DESTINATION cmake
)

# Install the configuration files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/ContainerConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/ContainerConfigVersion.cmake"
    DESTINATION cmake
)

# Install the exported targets
install(EXPORT ContainerTargets
    FILE ContainerTargets.cmake
    NAMESPACE Container::
    DESTINATION cmake
)
