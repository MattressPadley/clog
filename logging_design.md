# Unified CLog Library Design

## Overview

This document outlines the design for a unified, lightweight C++ logging library called **CLog**. The library is designed as a header-only, cross-platform solution that can be integrated into any C++ project via git submodules, providing consistent logging across embedded systems (Arduino/ESP32/RP2040) and desktop platforms.

## Design Philosophy

**CLog** is built around these core principles:
- **Universal**: Works on any C++17 platform without modification
- **Lightweight**: Header-only with zero dependencies beyond standard library
- **Simple**: Single include, intuitive API, minimal configuration
- **Flexible**: Supports both standalone and callback-based integration
- **Performance**: Minimal overhead suitable for real-time embedded systems

## CLog Library Architecture

### Repository Structure
```
clog/
├── include/clog/
│   ├── log.hpp          # Single header - complete library
│   ├── platform.hpp     # Platform detection and abstraction
│   └── config.hpp       # Compile-time configuration options
├── examples/
│   ├── arduino/         # Arduino IDE example
│   ├── esp32/           # ESP32 PlatformIO example
│   ├── desktop/         # Desktop CMake example
│   └── callback/        # Parent application integration example
├── tests/
│   ├── unit/            # Cross-platform unit tests
│   └── integration/     # Platform-specific integration tests
├── CMakeLists.txt       # CMake integration for desktop projects
├── library.json         # PlatformIO library manifest
├── library.properties   # Arduino IDE library properties
├── README.md           # Usage documentation and examples
└── LICENSE             # MIT or similar permissive license
```

### Core API Design

**Single Include Header** (`include/clog/log.hpp`):
```cpp
#pragma once
#include <cstdarg>
#include <cstdio>

// Platform detection
#if defined(ARDUINO) || defined(ESP32) || defined(ESP_PLATFORM)
    #define CLOG_PLATFORM_EMBEDDED
    #ifdef ARDUINO
        #include <Arduino.h>
        #define CLOG_PLATFORM_ARDUINO
    #endif
#elif defined(_WIN32) || defined(__linux__) || defined(__APPLE__)
    #define CLOG_PLATFORM_DESKTOP
    #include <iostream>
#else
    #define CLOG_PLATFORM_UNKNOWN
#endif

namespace clog {

enum class Level {
    OFF = 0,
    ERROR = 1,
    WARN = 2, 
    INFO = 3,
    DEBUG = 4,
    TRACE = 5
};

class Logger {
public:
    // Core API
    static void log(Level level, const char* tag, const char* format, ...);
    static void setLevel(Level level);
    static Level getLevel();
    
    // Callback integration for parent applications
    using Callback = void(*)(Level level, const char* tag, const char* message);
    static void setCallback(Callback callback);
    static void enableDirectOutput(bool enabled = true);
    
    // Platform-specific initialization (optional)
    static void init();
    
    // Convenience methods
    static void error(const char* tag, const char* format, ...);
    static void warn(const char* tag, const char* format, ...);
    static void info(const char* tag, const char* format, ...);
    static void debug(const char* tag, const char* format, ...);
    static void trace(const char* tag, const char* format, ...);
    
private:
    static Level currentLevel;
    static Callback logCallback;
    static bool directOutput;
    static void output(Level level, const char* tag, const char* message);
    static const char* levelToString(Level level);
    static const char* levelToColor(Level level);
};

} // namespace clog

// Convenience macros for easy usage
#define CLOG_ERROR(tag, fmt, ...)   clog::Logger::error(tag, fmt, ##__VA_ARGS__)
#define CLOG_WARN(tag, fmt, ...)    clog::Logger::warn(tag, fmt, ##__VA_ARGS__)
#define CLOG_INFO(tag, fmt, ...)    clog::Logger::info(tag, fmt, ##__VA_ARGS__)
#define CLOG_DEBUG(tag, fmt, ...)   clog::Logger::debug(tag, fmt, ##__VA_ARGS__)
#define CLOG_TRACE(tag, fmt, ...)   clog::Logger::trace(tag, fmt, ##__VA_ARGS__)

// Compile-time level filtering
#ifndef CLOG_LEVEL
    #define CLOG_LEVEL 3  // Default to INFO
#endif

#if CLOG_LEVEL < 1
    #undef CLOG_ERROR
    #define CLOG_ERROR(tag, fmt, ...) ((void)0)
#endif
#if CLOG_LEVEL < 2
    #undef CLOG_WARN
    #define CLOG_WARN(tag, fmt, ...) ((void)0)
#endif
#if CLOG_LEVEL < 3
    #undef CLOG_INFO
    #define CLOG_INFO(tag, fmt, ...) ((void)0)
#endif
#if CLOG_LEVEL < 4
    #undef CLOG_DEBUG
    #define CLOG_DEBUG(tag, fmt, ...) ((void)0)
#endif
#if CLOG_LEVEL < 5
    #undef CLOG_TRACE
    #define CLOG_TRACE(tag, fmt, ...) ((void)0)
#endif

// Implementation (header-only)
namespace clog {

// Static member definitions
Level Logger::currentLevel = Level::INFO;
Logger::Callback Logger::logCallback = nullptr;
bool Logger::directOutput = true;

void Logger::log(Level level, const char* tag, const char* format, ...) {
    if (level > currentLevel) return;
    
    char buffer[512];  // Fixed size for embedded compatibility
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    output(level, tag, buffer);
}

void Logger::output(Level level, const char* tag, const char* message) {
    if (logCallback) {
        logCallback(level, tag, message);
    } else if (directOutput) {
        const char* levelStr = levelToString(level);
        
#ifdef CLOG_PLATFORM_ARDUINO
        Serial.printf("[%s] %s: %s\n", levelStr, tag, message);
#elif defined(CLOG_PLATFORM_DESKTOP)
        const char* color = levelToColor(level);
        std::cout << color << "[" << levelStr << "] " << tag << ": " 
                  << message << "\033[0m" << std::endl;
#else
        printf("[%s] %s: %s\n", levelStr, tag, message);
#endif
    }
}

void Logger::setCallback(Callback callback) {
    logCallback = callback;
    directOutput = (callback == nullptr);
}

const char* Logger::levelToString(Level level) {
    switch (level) {
        case Level::ERROR: return "ERROR";
        case Level::WARN:  return "WARN ";
        case Level::INFO:  return "INFO ";
        case Level::DEBUG: return "DEBUG";
        case Level::TRACE: return "TRACE";
        default: return "?????";
    }
}

const char* Logger::levelToColor(Level level) {
    switch (level) {
        case Level::ERROR: return "\033[91m";  // Bright red
        case Level::WARN:  return "\033[93m";  // Bright yellow
        case Level::INFO:  return "\033[92m";  // Bright green
        case Level::DEBUG: return "\033[94m";  // Bright blue
        case Level::TRACE: return "\033[90m";  // Dark gray
        default: return "\033[0m";
    }
}

// Convenience method implementations
void Logger::error(const char* tag, const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[512];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    output(Level::ERROR, tag, buffer);
}

void Logger::warn(const char* tag, const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[512];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    output(Level::WARN, tag, buffer);
}

void Logger::info(const char* tag, const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[512];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    output(Level::INFO, tag, buffer);
}

void Logger::debug(const char* tag, const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[512];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    output(Level::DEBUG, tag, buffer);
}

void Logger::trace(const char* tag, const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[512];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    output(Level::TRACE, tag, buffer);
}

void Logger::setLevel(Level level) { currentLevel = level; }
Level Logger::getLevel() { return currentLevel; }
void Logger::enableDirectOutput(bool enabled) { directOutput = enabled; }
void Logger::init() {
    // Platform-specific initialization if needed
#ifdef CLOG_PLATFORM_ARDUINO
    // Serial already initialized by Arduino framework
#endif
}

} // namespace clog
```

## Usage Patterns

### Standalone Library Development
When developing or testing libraries in isolation:

```cpp
#include <clog/log.hpp>

// In your library tests or examples
int main() {
    clog::Logger::setLevel(clog::Level::DEBUG);
    
    CLOG_INFO("Test", "Starting library tests");
    CLOG_DEBUG("Parser", "Processing message: %s", msg.c_str());
    CLOG_ERROR("Hardware", "Failed to initialize device %d", deviceId);
    
    return 0;
}
```

**Output (Desktop with color)**:
```
[INFO ] Test: Starting library tests
[DEBUG] Parser: Processing message: /page1/fader1
[ERROR] Hardware: Failed to initialize device 2
```

### Parent Application Integration
When your library is used within a larger application:

```cpp
// In sk-slide library
#include <clog/log.hpp>

void SlideManager::initialize() {
    CLOG_INFO("SlideManager", "Initializing %d slides", numSlides);
    
    for (int i = 0; i < numSlides; i++) {
        if (!slides[i].calibrate()) {
            CLOG_ERROR("SlideManager", "Slide %d calibration failed", i);
            return false;
        }
        CLOG_DEBUG("SlideManager", "Slide %d calibrated successfully", i);
    }
}

// In parent application (Slide Kick ESP32 firmware)
void clogCallback(clog::Level level, const char* tag, const char* message) {
    // Route clog messages to your main logging system
    const char* levelStr = clogLevelToString(level);
    Serial.printf("[CLOG:%s] %s: %s\n", levelStr, tag, message);
}

void setup() {
    Serial.begin(115200);
    
    // Register callback to capture all library logging
    clog::Logger::setCallback(clogCallback);
    clog::Logger::setLevel(clog::Level::DEBUG);
    
    // Library logging now routes through your callback
    slideManager.initialize();
}
```

## Project Integration Examples

### Slide Kick Firmware Integration

**ESP32 Main Firmware** (`firmware/esp32/src/main.cpp`):
```cpp
#include <clog/log.hpp>

// Create unified logging system for all libraries
void slideKickLogHandler(clog::Level level, const char* tag, const char* message) {
    // Add categories for organization
    const char* category = "UNKNOWN";
    if (strstr(tag, "SPI") || strstr(tag, "DMA")) category = "SPI";
    else if (strstr(tag, "OSC") || strstr(tag, "Network")) category = "OSC";
    else if (strstr(tag, "Slide") || strstr(tag, "ADC")) category = "HARDWARE";
    
    const char* levelStr = "";
    switch (level) {
        case clog::Level::ERROR: levelStr = "ERROR"; break;
        case clog::Level::WARN:  levelStr = "WARN "; break;
        case clog::Level::INFO:  levelStr = "INFO "; break;
        case clog::Level::DEBUG: levelStr = "DEBUG"; break;
        case clog::Level::TRACE: levelStr = "TRACE"; break;
    }
    
    Serial.printf("[%s:%s] %s: %s\n", levelStr, category, tag, message);
}

void setup() {
    Serial.begin(115200);
    
    // Configure unified logging for all libraries and firmware
    clog::Logger::setCallback(slideKickLogHandler);
    clog::Logger::setLevel(clog::Level::DEBUG);
    
    CLOG_INFO("System", "Slide Kick ESP32 firmware starting...");
    
    // All library logging now routes through our handler
    initializeOSC();
    initializeSPI();
}

void initializeSPI() {
    CLOG_DEBUG("SPI", "Configuring DMA buffers");
    // SPI initialization code...
    CLOG_INFO("SPI", "SPI master initialized at 500Hz");
}
```

**RP2040 Firmware** (`firmware/rp2040/src/main.cpp`):
```cpp
#include <clog/log.hpp>

void rp2040LogHandler(clog::Level level, const char* tag, const char* message) {
    const char* levelStr = (level == clog::Level::ERROR) ? "ERROR" :
                          (level == clog::Level::WARN)  ? "WARN " :
                          (level == clog::Level::INFO)  ? "INFO " :
                          (level == clog::Level::DEBUG) ? "DEBUG" : "TRACE";
    
    printf("[%s] %s: %s\n", levelStr, tag, message);
}

int main() {
    stdio_init_all();
    
    clog::Logger::setCallback(rp2040LogHandler);
    clog::Logger::setLevel(clog::Level::DEBUG);
    
    CLOG_INFO("System", "Slide Kick RP2040 firmware starting...");
    
    // Initialize hardware with logging
    initializeSlides();
    
    while (true) {
        // Main loop with logging
        processSPICommands();
        sleep_ms(2); // 500Hz rate
    }
}
```

### Library Integration Examples

**SK-Slide Library Usage**:
```cpp
// In libs/sk-slide/src/slide_manager.cpp
#include <clog/log.hpp>

bool SlideManager::calibrateSlide(int slideId) {
    CLOG_DEBUG("SlideManager", "Starting calibration for slide %d", slideId);
    
    if (!slides[slideId].moveToPosition(0)) {
        CLOG_ERROR("SlideManager", "Failed to move slide %d to zero position", slideId);
        return false;
    }
    
    uint16_t zeroReading = slides[slideId].readADC();
    CLOG_TRACE("SlideManager", "Slide %d zero reading: %d", slideId, zeroReading);
    
    if (!slides[slideId].moveToPosition(4095)) {
        CLOG_ERROR("SlideManager", "Failed to move slide %d to max position", slideId);
        return false;
    }
    
    uint16_t maxReading = slides[slideId].readADC();
    CLOG_INFO("SlideManager", "Slide %d calibrated: zero=%d, max=%d", slideId, zeroReading, maxReading);
    
    return true;
}
```

**SK-OAL Library Usage**:
```cpp
// In libs/sk-oal/src/osc_parser.cpp
#include <clog/log.hpp>

OscMessage OscParser::parseMessage(const uint8_t* data, size_t length) {
    CLOG_TRACE("OscParser", "Parsing OSC message of %zu bytes", length);
    
    if (length < 4) {
        CLOG_ERROR("OscParser", "Message too short: %zu bytes", length);
        return OscMessage();
    }
    
    std::string address = parseAddress(data);
    CLOG_DEBUG("OscParser", "Parsed OSC address: %s", address.c_str());
    
    if (address.empty()) {
        CLOG_WARN("OscParser", "Empty or invalid OSC address");
        return OscMessage();
    }
    
    CLOG_INFO("OscParser", "Successfully parsed OSC message: %s", address.c_str());
    return OscMessage(address, parseArguments(data + address.length()));
}
```

## Build System Integration

### Git Submodule Integration

**Adding CLog to Projects**:
```bash
# Add clog as a submodule to any C++ project
cd your-project
git submodule add https://github.com/your-org/clog.git external/clog
git submodule update --init --recursive
```

### Slide Kick Project Integration

**Main Project** (`CMakeLists.txt` or `platformio.ini`):
```cmake
# CMake integration
add_subdirectory(external/clog)
target_link_libraries(slide_kick_firmware clog)
target_compile_definitions(slide_kick_firmware PRIVATE CLOG_LEVEL=4)
```

```ini
; PlatformIO integration
[env:esp32-s3-devkitc-1]
lib_deps = 
    file://external/clog
build_flags =
    -DCLOG_LEVEL=4  ; Debug level logging
    -Iexternal/clog/include
```

### Library Integration

**SK-Slide Library** (`libs/sk-slide/CMakeLists.txt`):
```cmake
# Add clog as dependency
find_path(CLOG_INCLUDE_DIR 
    NAMES clog/log.hpp
    PATHS ${CMAKE_SOURCE_DIR}/../../external/clog/include
)

if(CLOG_INCLUDE_DIR)
    target_include_directories(sk-slide PUBLIC ${CLOG_INCLUDE_DIR})
    target_compile_definitions(sk-slide PRIVATE CLOG_AVAILABLE=1)
else()
    message(WARNING "CLog not found - logging disabled")
    target_compile_definitions(sk-slide PRIVATE CLOG_AVAILABLE=0)
endif()
```

**SK-OAL Library** (`libs/sk-oal/platformio.ini`):
```ini
[env]
build_flags = 
    -Iexternal/clog/include
    -DCLOG_LEVEL=3
    -DCLOG_AVAILABLE=1

[env:native]
; Desktop development
platform = native

[env:esp32dev]
; Embedded development  
platform = espressif32
framework = arduino
```

### Optional CLog Integration Pattern

For libraries that may or may not have CLog available:

**Conditional Logging Header** (`libs/sk-slide/include/logging.hpp`):
```cpp
#pragma once

// Check if CLog is available
#if CLOG_AVAILABLE
    #include <clog/log.hpp>
    #define LIB_LOG_ERROR(tag, fmt, ...)   CLOG_ERROR(tag, fmt, ##__VA_ARGS__)
    #define LIB_LOG_WARN(tag, fmt, ...)    CLOG_WARN(tag, fmt, ##__VA_ARGS__)
    #define LIB_LOG_INFO(tag, fmt, ...)    CLOG_INFO(tag, fmt, ##__VA_ARGS__)
    #define LIB_LOG_DEBUG(tag, fmt, ...)   CLOG_DEBUG(tag, fmt, ##__VA_ARGS__)
    #define LIB_LOG_TRACE(tag, fmt, ...)   CLOG_TRACE(tag, fmt, ##__VA_ARGS__)
#else
    // Fallback to printf or no-op
    #ifdef ARDUINO
        #include <Arduino.h>
        #define LIB_LOG_ERROR(tag, fmt, ...)   Serial.printf("[ERROR] %s: " fmt "\n", tag, ##__VA_ARGS__)
        #define LIB_LOG_WARN(tag, fmt, ...)    Serial.printf("[WARN ] %s: " fmt "\n", tag, ##__VA_ARGS__)
        #define LIB_LOG_INFO(tag, fmt, ...)    Serial.printf("[INFO ] %s: " fmt "\n", tag, ##__VA_ARGS__)
        #define LIB_LOG_DEBUG(tag, fmt, ...)   ((void)0)  // Disabled
        #define LIB_LOG_TRACE(tag, fmt, ...)   ((void)0)  // Disabled
    #else
        #include <cstdio>
        #define LIB_LOG_ERROR(tag, fmt, ...)   printf("[ERROR] %s: " fmt "\n", tag, ##__VA_ARGS__)
        #define LIB_LOG_WARN(tag, fmt, ...)    printf("[WARN ] %s: " fmt "\n", tag, ##__VA_ARGS__)
        #define LIB_LOG_INFO(tag, fmt, ...)    printf("[INFO ] %s: " fmt "\n", tag, ##__VA_ARGS__)
        #define LIB_LOG_DEBUG(tag, fmt, ...)   ((void)0)  // Disabled
        #define LIB_LOG_TRACE(tag, fmt, ...)   ((void)0)  // Disabled
    #endif
#endif
```

**Usage in Library Code**:
```cpp
// In any library source file
#include "logging.hpp"

void SomeLibraryFunction() {
    LIB_LOG_INFO("LibraryName", "Function starting");
    
    if (error_condition) {
        LIB_LOG_ERROR("LibraryName", "Something failed: %d", error_code);
        return;
    }
    
    LIB_LOG_DEBUG("LibraryName", "Processing %d items", count);
}
```

  // Enable debug logging for testing
