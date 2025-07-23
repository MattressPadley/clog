cmake_minimum_required(VERSION 3.13)

# Include the Pico SDK only if not already available
if(NOT COMMAND pico_sdk_init)
  # Check if PICO_SDK_PATH is set, otherwise download to build folder
  if(NOT DEFINED ENV{PICO_SDK_PATH})
    set(PICO_SDK_PATH ${CMAKE_CURRENT_BINARY_DIR}/pico-sdk)
    if(NOT EXISTS ${PICO_SDK_PATH})
      message(STATUS "Downloading Pico SDK to ${PICO_SDK_PATH}")
      execute_process(
                COMMAND git clone --depth 1 --branch 2.1.1 https://github.com/raspberrypi/pico-sdk.git ${PICO_SDK_PATH}
                RESULT_VARIABLE GIT_RESULT
            )
      if(NOT GIT_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to download Pico SDK")
      endif()
      execute_process(
                COMMAND git submodule update --init --recursive
                WORKING_DIRECTORY ${PICO_SDK_PATH}
                RESULT_VARIABLE GIT_RESULT
            )
      if(NOT GIT_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to initialize Pico SDK submodules")
      endif()
    endif()
    include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
  else()
    include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
  endif()
endif()

project(clog_pico_example)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Initialize the Pico SDK
pico_sdk_init()

# Create the executable
add_executable(pico_example pico_example.cpp)

# Add the CLog include directory
target_include_directories(pico_example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include)

# Link to pico_stdlib (required for Pico SDK functionality)
target_link_libraries(pico_example pico_stdlib)

# Enable USB serial output
pico_enable_stdio_usb(pico_example 1)
pico_enable_stdio_uart(pico_example 0)

# Create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(pico_example)