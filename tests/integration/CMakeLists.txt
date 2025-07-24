cmake_minimum_required(VERSION 3.12)

# Integration tests for CLog library
project(clog_integration_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Test simple integration (without complex lambdas)
add_executable(test_simple_integration test_simple_integration.cpp)
target_include_directories(test_simple_integration PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
target_compile_definitions(test_simple_integration PRIVATE CLOG_LEVEL=5)  # Enable all log levels for testing
add_test(NAME test_simple_integration COMMAND test_simple_integration)

# Test library system integration
add_executable(test_library_system test_library_system.cpp)
target_include_directories(test_library_system PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
target_compile_definitions(test_library_system PRIVATE CLOG_LEVEL=5)  # Enable all log levels for testing
add_test(NAME test_library_system COMMAND test_library_system)

# Create a target to run integration tests
add_custom_target(run_integration_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose -R "test_simple_integration|test_library_system"
    DEPENDS test_simple_integration test_library_system
    COMMENT "Running CLog integration tests"
)

# Set test properties
set_tests_properties(test_simple_integration PROPERTIES
    TIMEOUT 60
    LABELS "integration;simple"
)

set_tests_properties(test_library_system PROPERTIES
    TIMEOUT 60
    LABELS "integration;library"
)