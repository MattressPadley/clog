cmake_minimum_required(VERSION 3.12)

# Unit tests for CLog library
project(clog_unit_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Test logger functionality
add_executable(test_logger test_logger.cpp)
target_include_directories(test_logger PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
target_compile_definitions(test_logger PRIVATE CLOG_LEVEL=5)  # Enable all log levels for testing
add_test(NAME test_logger COMMAND test_logger)

# Test platform functionality
add_executable(test_platform test_platform.cpp)
target_include_directories(test_platform PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
target_compile_definitions(test_platform PRIVATE CLOG_LEVEL=5)  # Enable all log levels for testing
add_test(NAME test_platform COMMAND test_platform)

# Create a target to run unit tests
add_custom_target(run_unit_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose -R "test_logger|test_platform"
    DEPENDS test_logger test_platform
    COMMENT "Running CLog unit tests"
)

# Set test properties
set_tests_properties(test_logger PROPERTIES
    TIMEOUT 30
    LABELS "unit;logger"
)

set_tests_properties(test_platform PROPERTIES
    TIMEOUT 30
    LABELS "unit;platform"
)