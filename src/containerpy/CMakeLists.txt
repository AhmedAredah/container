# Set minimum required CMake version
cmake_minimum_required(VERSION 3.24)

set(PYTHON_LIB_NAME "ContainerPy")

# Find required packages
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Sql)
find_package(pybind11 REQUIRED CONFIG HINTS ${PYBIND11_HINTS_PATH})

# Define source files
set(BINDING_FILES
    bindcontainer.cpp
    containerext.cpp
    packageext.cpp
    containermapext.cpp
)

# Define header files
set(BINDING_HEADERS
    containerext.h
    packageext.h
    containermapext.h
)

# Create Python module
pybind11_add_module(${PYTHON_LIB_NAME} MODULE ${BINDING_FILES} ${BINDING_HEADERS})

# Set target properties
set_target_properties(${PYTHON_LIB_NAME} PROPERTIES 
    OUTPUT_NAME "ContainerPy"
    DEBUG_POSTFIX ""
)

# Set the Python module as position independent code
set_target_properties(${PYTHON_LIB_NAME} PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

# Add include directories
target_include_directories(${PYTHON_LIB_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/containerlib
)

# Link against the main library and Qt
target_link_libraries(${PYTHON_LIB_NAME} PRIVATE
    Container
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Sql
)

# Link against Python if not on Apple
if(NOT APPLE)
    target_link_libraries(${PYTHON_LIB_NAME} PRIVATE Python::Python)
endif()

# Add dependency to ensure Container is built first
add_dependencies(${PYTHON_LIB_NAME} Container)

# Copy dependencies to output directory
add_custom_command(TARGET ${PYTHON_LIB_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Container> $<TARGET_FILE_DIR:${PYTHON_LIB_NAME}>
)

# Define install directory
set(INSTALL_DIR "${CMAKE_BINARY_DIR}/installing/containerpy")
file(MAKE_DIRECTORY ${INSTALL_DIR})

# Install Python module and dependencies
install(TARGETS ${PYTHON_LIB_NAME}
    DESTINATION ${INSTALL_DIR}
    COMPONENT Runtime
)

install(TARGETS Container
    DESTINATION ${INSTALL_DIR}
    COMPONENT Runtime
)

# Install Qt dependencies based on platform
if(WIN32)
    # Windows-specific Qt deployment
    get_target_property(QT_CORE_DLL Qt6::Core IMPORTED_LOCATION_RELEASE)
    get_target_property(QT_SQL_DLL Qt6::Sql IMPORTED_LOCATION_RELEASE)
    
    install(FILES
        "${QT_CORE_DLL}"
        "${QT_SQL_DLL}"
        DESTINATION ${INSTALL_DIR}
    )
elseif(APPLE)
    # macOS-specific Qt deployment
    get_target_property(QT_CORE_FW Qt6::Core IMPORTED_LOCATION)
    get_target_property(QT_SQL_FW Qt6::Sql IMPORTED_LOCATION)
    
    install(FILES
        "${QT_CORE_FW}"
        "${QT_SQL_FW}"
        DESTINATION ${INSTALL_DIR}/Frameworks
    )
    
    set_target_properties(${PYTHON_LIB_NAME} PROPERTIES
        INSTALL_RPATH "@loader_path/Frameworks"
        BUILD_WITH_INSTALL_RPATH TRUE
    )
else()
    # Linux-specific Qt deployment
    get_target_property(QT_CORE_SO Qt6::Core IMPORTED_LOCATION)
    get_target_property(QT_SQL_SO Qt6::Sql IMPORTED_LOCATION)
    
    install(FILES
        "${QT_CORE_SO}"
        "${QT_SQL_SO}"
        DESTINATION ${INSTALL_DIR}
    )
endif()

# Install Python package files
install(FILES
    "${CMAKE_SOURCE_DIR}/python/containerpy/ContainerPy.pyi"
    "${CMAKE_SOURCE_DIR}/python/containerpy/__init__.py"
    DESTINATION ${INSTALL_DIR}
)

# Install setup.py to parent directory
# install(FILES
#     "${CMAKE_SOURCE_DIR}/python/setup.py"
#     DESTINATION ${CMAKE_SOURCE_DIR}
# )

# Print configuration summary
message(STATUS "")
message(STATUS "Python Bindings Configuration")
message(STATUS "----------------------------")
message(STATUS "Python version:      ${Python_VERSION}")
message(STATUS "Python executable:   ${Python_EXECUTABLE}")
message(STATUS "pybind11 version:    ${pybind11_VERSION}")
message(STATUS "Install directory:   ${INSTALL_DIR}")
message(STATUS "")
