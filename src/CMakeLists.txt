add_subdirectory(containerlib)

# If Python bindings are to be built, load the subproject
if(BUILD_PYTHON_PACKAGE)
    add_subdirectory(containerpy)
endif()
