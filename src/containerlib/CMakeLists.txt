cmake_minimum_required(VERSION 3.14)

project(${Container_NAME} VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Sql)

add_library(${Container_NAME} SHARED
  Container_global.h
  package.h package.cpp
  container.h container.cpp
  containermap.h containermap.cpp
)


# Ensure that the library has a 'd' postfix in Debug builds
set_target_properties(${Container_NAME} PROPERTIES
  DEBUG_POSTFIX "d"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release
)

target_link_libraries(${Container_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Sql
)

# Export the include directories for other projects to use
target_include_directories(${Container_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include/Container>
)

target_compile_definitions(${Container_NAME} PRIVATE CONTAINER_LIBRARY)


# Set the install directories
install(TARGETS ${Container_NAME}
    EXPORT ContainerTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(FILES
    container.h
    package.h
    Container_global.h
    containermap.h
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
    "${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in"
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
