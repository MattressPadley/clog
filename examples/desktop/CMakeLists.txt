cmake_minimum_required(VERSION 3.12)
project(clog_desktop_example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(desktop_example desktop_example.cpp)

# Add the parent directory to find clog headers
target_include_directories(desktop_example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include)