#pragma once

// Platform detection utilities for the CLog library
// This header provides standardized platform detection macros and utilities

// Simplified platform configuration - defaults to desktop behavior
// Platform-specific overrides should be set via CMake target_compile_definitions

// Default desktop behavior (can be overridden by CMake)
#ifndef CLOG_HAS_COLOR_SUPPORT
    #define CLOG_HAS_COLOR_SUPPORT 1  // Desktop default: colors enabled
#endif

#ifndef CLOG_HAS_PRINTF_SUPPORT  
    #define CLOG_HAS_PRINTF_SUPPORT 0  // Desktop default: use std::cout, not printf
#endif

// Include appropriate headers based on platform defines (set by CMake)
#if defined(CLOG_PLATFORM_ESP32) || defined(CLOG_PLATFORM_ESP8266) || defined(CLOG_PLATFORM_ESP_IDF)
    #include <esp_log.h>
#elif defined(CLOG_PLATFORM_ARDUINO)
    #include <Arduino.h>
#elif defined(CLOG_PLATFORM_RP2040_SDK)
    #include <pico/stdio.h>
#else
    // Default to desktop includes
    #include <iostream>
#endif

// Platform-specific includes and utilities
namespace clogger {
namespace platform {

// Initialize platform-specific features
inline void init() {
#if defined(CLOG_PLATFORM_RP2040_SDK)
    stdio_init_all();
#elif defined(CLOG_PLATFORM_ESP_IDF)
    // ESP-IDF logging is already initialized
#endif
    // Default (desktop) and other platforms: no initialization needed
}

// Get platform name as string
inline const char* getName() {
#ifdef CLOG_PLATFORM_ESP32
    return "ESP32";
#elif defined(CLOG_PLATFORM_ESP8266)
    return "ESP8266";
#elif defined(CLOG_PLATFORM_RP2040_ARDUINO)
    return "RP2040-Arduino";
#elif defined(CLOG_PLATFORM_RP2040_SDK)
    return "RP2040-SDK";
#elif defined(CLOG_PLATFORM_AVR)
    return "Arduino-AVR";
#elif defined(CLOG_PLATFORM_ESP_IDF)
    return "ESP-IDF";
#elif defined(CLOG_PLATFORM_WINDOWS)
    return "Windows";
#elif defined(CLOG_PLATFORM_LINUX)
    return "Linux";
#elif defined(CLOG_PLATFORM_MACOS)
    return "macOS";
#else
    return "Desktop";  // Default platform name
#endif
}

// Check if platform supports color output
inline constexpr bool hasColorSupport() {
    return CLOG_HAS_COLOR_SUPPORT;
}

// Check if platform has printf-style formatting
inline constexpr bool hasPrintfSupport() {
    return CLOG_HAS_PRINTF_SUPPORT;
}

// Get default buffer size for the platform
inline constexpr size_t getDefaultBufferSize() {
#if defined(CLOG_PLATFORM_ESP32) || defined(CLOG_PLATFORM_ESP8266) || defined(CLOG_PLATFORM_AVR) || defined(CLOG_PLATFORM_RP2040_ARDUINO) || defined(CLOG_PLATFORM_RP2040_SDK)
    return 256;  // Smaller buffer for embedded systems
#else
    return 1024; // Larger buffer for desktop systems (default)
#endif
}

// Check if we're running on an embedded platform
inline constexpr bool isEmbedded() {
#if defined(CLOG_PLATFORM_ESP32) || defined(CLOG_PLATFORM_ESP8266) || defined(CLOG_PLATFORM_AVR) || defined(CLOG_PLATFORM_RP2040_ARDUINO) || defined(CLOG_PLATFORM_RP2040_SDK) || defined(CLOG_PLATFORM_ESP_IDF)
    return true;
#else
    return false;  // Default (desktop) is not embedded
#endif
}

} // namespace platform
} // namespace clogger