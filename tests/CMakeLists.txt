cmake_minimum_required(VERSION 3.12)
project(clog_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tests use direct include paths

# Enable testing
enable_testing()

# Add subdirectories for different test types
add_subdirectory(unit)
add_subdirectory(integration)

# Create a target to run all tests
add_custom_target(run_all_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS run_unit_tests run_integration_tests
    COMMENT "Running all CLog tests"
)