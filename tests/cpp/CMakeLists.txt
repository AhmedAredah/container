# Find GTest package
find_package(GTest QUIET)

if(GTest_FOUND)
    # Add test executable
    add_executable(container_tests
        test_container.cpp
    )

    # Link against GTest and your library
    target_link_libraries(container_tests
        PRIVATE
            Container
            GTest::GTest
            GTest::Main
    )

    # Add test to CTest
    add_test(NAME container_tests COMMAND container_tests)
else()
    message(STATUS "GTest not found - C++ tests will not be built")
endif()