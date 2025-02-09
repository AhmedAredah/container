# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Test)

# Add test executable
add_executable(container_tests
    test_container.cpp
)

# Link against Qt6 Test
target_link_libraries(container_tests
    PRIVATE
    Qt6::Core
    Qt6::Test
    Container
)

# Enable testing
enable_testing()

# Add test to CTest
add_test(NAME container_tests COMMAND container_tests)
