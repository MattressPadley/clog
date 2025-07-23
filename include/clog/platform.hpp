#pragma once

// Platform detection utilities for the CLog library
// This header provides standardized platform detection macros and utilities

// Primary platform detection
#if defined(ARDUINO) || defined(ESP32) || defined(ESP_PLATFORM)
    #define CLOG_PLATFORM_EMBEDDED
    
    // Arduino-specific detection
    #ifdef ARDUINO
        #define CLOG_PLATFORM_ARDUINO
        #include <Arduino.h>
        
        // Arduino board-specific detection
        #if defined(ESP32)
            #define CLOG_PLATFORM_ESP32
        #elif defined(ESP8266)
            #define CLOG_PLATFORM_ESP8266
        #elif defined(__AVR__)
            #define CLOG_PLATFORM_AVR
        #elif defined(ARDUINO_ARCH_RP2040)
            #define CLOG_PLATFORM_RP2040_ARDUINO
        #endif
    #endif
    
    // ESP-IDF specific detection
    #if defined(ESP_PLATFORM) && !defined(ARDUINO)
        #define CLOG_PLATFORM_ESP_IDF
        #include <esp_log.h>
    #endif
    
    // Raspberry Pi Pico SDK detection
    #if defined(PICO_SDK_VERSION_MAJOR)
        #define CLOG_PLATFORM_RP2040_SDK
        #include <pico/stdio.h>
    #endif

#elif defined(_WIN32) || defined(__linux__) || defined(__APPLE__)
    #define CLOG_PLATFORM_DESKTOP
    #include <iostream>
    
    // Desktop OS-specific detection
    #ifdef _WIN32
        #define CLOG_PLATFORM_WINDOWS
    #elif defined(__linux__)
        #define CLOG_PLATFORM_LINUX
    #elif defined(__APPLE__)
        #define CLOG_PLATFORM_MACOS
    #endif
    
#else
    #define CLOG_PLATFORM_UNKNOWN
    #warning "Unknown platform detected - CLog may not function correctly"
#endif

// Feature detection
#if defined(CLOG_PLATFORM_DESKTOP) || defined(CLOG_PLATFORM_ESP_IDF)
    #define CLOG_HAS_COLOR_SUPPORT 1
#else
    #define CLOG_HAS_COLOR_SUPPORT 0
#endif

#if defined(CLOG_PLATFORM_ARDUINO) || defined(CLOG_PLATFORM_ESP_IDF)
    #define CLOG_HAS_PRINTF_SUPPORT 1
#else
    #define CLOG_HAS_PRINTF_SUPPORT 0
#endif

// Platform-specific includes and utilities
namespace clogger {
namespace platform {

// Initialize platform-specific features
inline void init() {
#ifdef CLOG_PLATFORM_ARDUINO
    // Serial is typically initialized by Arduino framework
    // but we can check if it needs initialization
#elif defined(CLOG_PLATFORM_RP2040_SDK)
    stdio_init_all();
#elif defined(CLOG_PLATFORM_ESP_IDF)
    // ESP-IDF logging is already initialized
#endif
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
    return "Unknown";
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
#ifdef CLOG_PLATFORM_EMBEDDED
    return 256;  // Smaller buffer for embedded systems
#else
    return 1024; // Larger buffer for desktop systems
#endif
}

// Check if we're running on an embedded platform
inline constexpr bool isEmbedded() {
#ifdef CLOG_PLATFORM_EMBEDDED
    return true;
#else
    return false;
#endif
}

} // namespace platform
} // namespace clogger