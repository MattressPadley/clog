# Quick Start Guide

This guide will get you up and running with CLog in just a few minutes. For detailed information, see the other documentation files.

## Basic Usage

The simplest way to use CLog is with compile-time configuration:

```cpp
// Configure at compile time (recommended approach)
#define CLOG_LEVEL 4  // DEBUG level - filters out TRACE at compile time
#include <clog/log.hpp>

int main() {
    // No setup needed - log level configured at compile time
    CLOG_ERROR("MyApp", "Something went wrong: %d", error_code);
    CLOG_WARN("MyApp", "Warning condition detected");
    CLOG_INFO("MyApp", "Application started");
    CLOG_DEBUG("MyApp", "Debug info: %s", debug_string);
    CLOG_TRACE("MyApp", "This is compiled out (level 5 > CLOG_LEVEL 4)");
    
    return 0;
}
```

## CMake Configuration (Recommended)

For better build system integration, configure via CMake:

```cmake
# Configure via CMake for better build system integration
add_executable(my_app main.cpp)
target_compile_definitions(my_app PRIVATE 
    CLOG_LEVEL=4                    # Debug level
    CLOG_PLATFORM=DESKTOP           # Explicit platform (optional)
)
target_link_libraries(my_app PRIVATE clog::clog)
```

```cpp
#include <clog/log.hpp>

int main() {
    // No setup needed - everything configured via CMake
    CLOG_INFO("MyApp", "Application started with CMake configuration");
    return 0;
}
```

## Integration with Parent Applications

CLog can route all logs through a callback for integration with existing logging systems:

```cpp
#include <clog/log.hpp>

void myLogCallback(clogger::Level level, const char* tag, const char* message) {
    // Route CLog messages to your existing logging system
    printf("[%s] %s: %s\n", levelToString(level), tag, message);
}

int main() {
    // Register callback to capture all library logging
    clogger::Logger::setCallback(myLogCallback);
    // Note: Log level configured at compile time via CLOG_LEVEL
    
    // All CLOG_* calls will now go through your callback
    CLOG_INFO("System", "Library integrated successfully");
    
    return 0;
}
```

## Common Patterns

### Desktop Development

```cpp
// Desktop development with colors enabled
#define CLOG_LEVEL 5  // TRACE level for development
#include <clog/log.hpp>

int main() {
    // Desktop defaults: colors enabled, std::cout output
    CLOG_INFO("App", "Desktop application started");
    
    // Configure tag colors for better visual distinction
    clogger::Logger::setTagColor("Database", clogger::Color::CLOG_BRIGHT_CYAN);
    clogger::Logger::setTagColor("Network", clogger::Color::CLOG_BRIGHT_MAGENTA);
    
    CLOG_INFO("Database", "This appears in cyan");
    CLOG_INFO("Network", "This appears in magenta");
    
    return 0;
}
```

### Library Development

```cpp
// In a library - set reasonable default if not configured
#ifndef CLOG_LEVEL
    #define CLOG_LEVEL 3  // INFO level default for libraries
#endif
#include <clog/log.hpp>

namespace MyLibrary {
    void init() {
        CLOG_INFO("MyLib", "Library initialized");
        CLOG_DEBUG("MyLib", "Debug info only shown if app sets DEBUG level");
    }
}
```

### Embedded Systems

```cmake
# For embedded platforms like ESP32
add_executable(esp32_app main.cpp)
target_compile_definitions(esp32_app PRIVATE 
    CLOG_LEVEL=2                    # WARN level for production
    CLOG_PLATFORM=ESP32             # Explicit platform
    CLOG_BUFFER_SIZE=256            # Smaller buffer for embedded
)
target_link_libraries(esp32_app PRIVATE clog::clog)
```

```cpp
#include <clog/log.hpp>

void setup() {
    // ESP32/Arduino setup
    Serial.begin(115200);
    CLOG_INFO("ESP32", "System initialized");
}

void loop() {
    // Your main loop
    CLOG_DEBUG("Loop", "Main loop iteration");
    delay(1000);
}
```

## Next Steps

- **[Installation Guide](installation.md)** - Detailed installation and CMake integration
- **[Configuration](configuration.md)** - Complete configuration options
- **[Platform Support](platform-support.md)** - Platform-specific setup guides
- **[API Reference](api-reference.md)** - Complete API documentation
- **[Multi-Library Projects](multi-library-projects.md)** - Best practices for complex projects

## Quick Reference

### Log Levels
- `CLOG_ERROR(tag, format, ...)` - Error level (1)
- `CLOG_WARN(tag, format, ...)` - Warning level (2)  
- `CLOG_INFO(tag, format, ...)` - Info level (3)
- `CLOG_DEBUG(tag, format, ...)` - Debug level (4)
- `CLOG_TRACE(tag, format, ...)` - Trace level (5)

### Common Configuration
- `CLOG_LEVEL=N` - Set maximum log level (0-5)
- `CLOG_BUFFER_SIZE=N` - Set message buffer size
- `CLOG_PLATFORM=PLATFORM` - Set target platform explicitly

### Common API Calls
- `clogger::Logger::setCallback(callback)` - Route logs to callback
- `clogger::Logger::setTagColor(tag, color)` - Set tag color (desktop)
- `clogger::Logger::enableTag(tag)` - Enable specific tag filtering