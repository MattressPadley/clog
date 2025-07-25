cmake_minimum_required(VERSION 3.12)
project(clog VERSION 0.1.1 LANGUAGES CXX)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the library as INTERFACE (header-only)
add_library(clog INTERFACE)

# Add include directories
target_include_directories(clog INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Specify the header files (for IDE support)
target_sources(clog INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/clog/log.hpp>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/clog/platform.hpp>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/clog/config.hpp>
)

# Create an alias for easier usage
add_library(clog::clog ALIAS clog)

# Platform configuration option
set(CLOG_PLATFORM "AUTO_DETECT" CACHE STRING "Target platform for CLog (AUTO_DETECT, ARDUINO, ESP32, ESP8266, RP2040_ARDUINO, RP2040_SDK, ESP_IDF, DESKTOP, WINDOWS, LINUX, MACOS)")
set_property(CACHE CLOG_PLATFORM PROPERTY STRINGS 
    "AUTO_DETECT" "ARDUINO" "ESP32" "ESP8266" "RP2040_ARDUINO" "RP2040_SDK" "ESP_IDF" "DESKTOP" "WINDOWS" "LINUX" "MACOS")

# Validate and configure platform
if(CLOG_PLATFORM STREQUAL "AUTO_DETECT")
    message(STATUS "CLog: Using automatic platform detection")
elseif(CLOG_PLATFORM STREQUAL "ARDUINO")
    target_compile_definitions(clog INTERFACE CLOG_PLATFORM_ARDUINO=1)
    message(STATUS "CLog: Configured for Arduino platform")
elseif(CLOG_PLATFORM STREQUAL "ESP32")
    target_compile_definitions(clog INTERFACE CLOG_PLATFORM_ESP32=1)
    message(STATUS "CLog: Configured for ESP32 platform")
elseif(CLOG_PLATFORM STREQUAL "ESP8266")
    target_compile_definitions(clog INTERFACE CLOG_PLATFORM_ESP8266=1)
    message(STATUS "CLog: Configured for ESP8266 platform")
elseif(CLOG_PLATFORM STREQUAL "RP2040_ARDUINO")
    target_compile_definitions(clog INTERFACE CLOG_PLATFORM_RP2040_ARDUINO=1)
    message(STATUS "CLog: Configured for RP2040-Arduino platform")
elseif(CLOG_PLATFORM STREQUAL "RP2040_SDK")
    target_compile_definitions(clog INTERFACE CLOG_PLATFORM_RP2040_SDK=1)
    message(STATUS "CLog: Configured for RP2040-SDK platform")
elseif(CLOG_PLATFORM STREQUAL "ESP_IDF")
    target_compile_definitions(clog INTERFACE CLOG_PLATFORM_ESP_IDF=1)
    message(STATUS "CLog: Configured for ESP-IDF platform")
elseif(CLOG_PLATFORM STREQUAL "DESKTOP")
    target_compile_definitions(clog INTERFACE CLOG_PLATFORM_DESKTOP=1)
    message(STATUS "CLog: Configured for Desktop platform")
elseif(CLOG_PLATFORM STREQUAL "WINDOWS")
    target_compile_definitions(clog INTERFACE CLOG_PLATFORM_WINDOWS=1)
    message(STATUS "CLog: Configured for Windows platform")
elseif(CLOG_PLATFORM STREQUAL "LINUX")
    target_compile_definitions(clog INTERFACE CLOG_PLATFORM_LINUX=1)
    message(STATUS "CLog: Configured for Linux platform")
elseif(CLOG_PLATFORM STREQUAL "MACOS")
    target_compile_definitions(clog INTERFACE CLOG_PLATFORM_MACOS=1)
    message(STATUS "CLog: Configured for macOS platform")
else()
    message(FATAL_ERROR "Invalid CLOG_PLATFORM value: ${CLOG_PLATFORM}. Valid options are: AUTO_DETECT, ARDUINO, ESP32, ESP8266, RP2040_ARDUINO, RP2040_SDK, ESP_IDF, DESKTOP, WINDOWS, LINUX, MACOS")
endif()

# Optional: Build examples if this is the main project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    option(CLOG_BUILD_EXAMPLES "Build CLog examples" ON)
    option(CLOG_BUILD_TESTS "Build CLog tests" ON)
else()
    option(CLOG_BUILD_EXAMPLES "Build CLog examples" OFF)
    option(CLOG_BUILD_TESTS "Build CLog tests" OFF)
endif()

# Build examples
if(CLOG_BUILD_EXAMPLES)
    add_subdirectory(examples/desktop)
    
    # Only build Pico example if Pico SDK is available
    if(DEFINED ENV{PICO_SDK_PATH} OR PICO_SDK_FETCH_FROM_GIT)
        option(CLOG_BUILD_PICO_EXAMPLE "Build CLog Pico example" ON)
        if(CLOG_BUILD_PICO_EXAMPLE)
            message(STATUS "Pico SDK detected, building Pico example")
            add_subdirectory(examples/pico)
        endif()
    else()
        message(STATUS "Pico SDK not found, skipping Pico example")
    endif()
endif()

# Build tests
if(CLOG_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation configuration
include(GNUInstallDirs)

# Install the library
install(TARGETS clog
    EXPORT clogTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)

# Install CMake config files
install(EXPORT clogTargets
    FILE clogTargets.cmake
    NAMESPACE clog::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/clog
)

# Create config file
include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/clogConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/clogConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/clog
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/clogConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/clogConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/clogConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/clog
)

# Export targets from build tree
export(EXPORT clogTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/clogTargets.cmake"
    NAMESPACE clog::
)

# Add to CMake registry
export(PACKAGE clog)