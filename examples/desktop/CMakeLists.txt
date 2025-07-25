cmake_minimum_required(VERSION 3.12)
project(clog_desktop_example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(desktop_example desktop_example.cpp)

# Add the parent directory to find clog headers
target_include_directories(desktop_example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include)

# Set compile-time log level to TRACE (5) for comprehensive example output
target_compile_definitions(desktop_example PRIVATE 
    CLOG_LEVEL=5
    # Desktop platform settings (these are the defaults, shown for clarity)
    # CLOG_HAS_COLOR_SUPPORT=1    # Desktop supports colors (default)
    # CLOG_HAS_PRINTF_SUPPORT=0   # Desktop uses std::cout (default)
)