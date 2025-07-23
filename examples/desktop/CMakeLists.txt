cmake_minimum_required(VERSION 3.12)
project(clog_desktop_example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find CLog library
find_package(clog REQUIRED PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../..)

add_executable(desktop_example desktop_example.cpp)
target_link_libraries(desktop_example clog::clog)