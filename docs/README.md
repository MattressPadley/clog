# CLog - Unified Logging Library

CLog is a lightweight, header-only C++ logging library designed for cross-platform compatibility across embedded systems (Arduino, ESP32, RP2040) and desktop platforms. It provides consistent logging APIs with callback support for integration into larger applications.

## Features

- **Header-only**: Single include, no compilation required
- **Cross-platform**: Works on Arduino, ESP32, RP2040, and desktop systems
- **Lightweight**: Minimal overhead suitable for embedded systems
- **Flexible**: Supports both direct output and callback-based integration
- **Configurable**: Compile-time log level filtering and buffer size configuration
- **Zero dependencies**: Only uses standard C++ library features

## Quick Start

### Basic Usage

```cpp
#include <clog/log.hpp>

int main() {
    // Set log level
    clog::Logger::setLevel(clog::Level::DEBUG);
    
    // Basic logging
    CLOG_ERROR("MyApp", "Something went wrong: %d", error_code);
    CLOG_WARN("MyApp", "Warning condition detected");
    CLOG_INFO("MyApp", "Application started");
    CLOG_DEBUG("MyApp", "Debug info: %s", debug_string);
    CLOG_TRACE("MyApp", "Detailed trace information");
    
    return 0;
}
```

### Integration with Parent Applications

```cpp
#include <clog/log.hpp>

void myLogCallback(clog::Level level, const char* tag, const char* message) {
    // Route CLog messages to your existing logging system
    printf("[%s] %s: %s\n", levelToString(level), tag, message);
}

int main() {
    // Register callback to capture all library logging
    clog::Logger::setCallback(myLogCallback);
    clog::Logger::setLevel(clog::Level::DEBUG);
    
    // All CLOG_* calls will now go through your callback
    CLOG_INFO("System", "Library integrated successfully");
    
    return 0;
}
```

## Supported Platforms

- **Desktop**: Windows, Linux, macOS
- **Arduino**: All Arduino-compatible boards
- **ESP32/ESP8266**: Both Arduino framework and ESP-IDF
- **Raspberry Pi Pico**: RP2040 with Arduino framework or Pico SDK

## Installation

### As Git Submodule

```bash
# Add to your project
git submodule add https://github.com/MattressPadley/clog.git external/clog
git submodule update --init --recursive
```

### CMake Integration

```cmake
# Add CLog to your CMakeLists.txt
add_subdirectory(external/clog)
target_link_libraries(your_target clog::clog)
```

### PlatformIO Integration

```ini
; Add to platformio.ini
[env:your_env]
lib_deps = 
    file://external/clog
build_flags =
    -DCLOG_LEVEL=4  ; Debug level
```

### Arduino IDE

1. Copy the `clog` folder to your Arduino libraries directory
2. Include in your sketch: `#include <clog/log.hpp>`

## Configuration

### Compile-time Configuration

```cpp
// Define before including clog/log.hpp
#define CLOG_LEVEL 4        // 0=OFF, 1=ERROR, 2=WARN, 3=INFO, 4=DEBUG, 5=TRACE
#define CLOG_BUFFER_SIZE 512 // Message buffer size
#include <clog/log.hpp>
```

### Build System Configuration

**CMake:**
```cmake
target_compile_definitions(your_target PRIVATE 
    CLOG_LEVEL=4
    CLOG_BUFFER_SIZE=256
)
```

**PlatformIO:**
```ini
build_flags = 
    -DCLOG_LEVEL=4
    -DCLOG_BUFFER_SIZE=256
```

## API Reference

### Log Levels

- `clog::Level::OFF` - No logging
- `clog::Level::ERROR` - Error messages only
- `clog::Level::WARN` - Warnings and errors
- `clog::Level::INFO` - Informational messages and above
- `clog::Level::DEBUG` - Debug messages and above
- `clog::Level::TRACE` - All messages including trace

### Logging Macros

```cpp
CLOG_ERROR(tag, format, ...)   // Error level
CLOG_WARN(tag, format, ...)    // Warning level
CLOG_INFO(tag, format, ...)    // Info level
CLOG_DEBUG(tag, format, ...)   // Debug level
CLOG_TRACE(tag, format, ...)   // Trace level
```

### Logger Methods

```cpp
// Set global log level
clog::Logger::setLevel(clog::Level::DEBUG);

// Get current log level
clog::Level level = clog::Logger::getLevel();

// Set callback for message handling
clog::Logger::setCallback(myCallback);

// Enable/disable direct output
clog::Logger::enableDirectOutput(true);

// Platform-specific initialization (usually not needed)
clog::Logger::init();
```

## Examples

The library includes comprehensive examples:

- **Desktop Example**: Basic usage with colored console output
- **Arduino Example**: Embedded system logging with sensor simulation
- **ESP32 Advanced**: Multi-task logging with WiFi and web server integration
- **Callback Integration**: Advanced integration with parent application logging systems

See the `examples/` directory for complete, runnable examples.

## Performance Considerations

### Embedded Systems

- Default buffer size: 256 bytes
- Fixed-size buffers prevent dynamic allocation
- Compile-time level filtering removes unused code
- Platform-specific optimizations for Arduino/ESP32

### Desktop Systems

- Default buffer size: 512 bytes
- Color-coded console output
- Thread-safe operation (planned for future release)

## Integration with Slide Kick

CLog is designed as a unified logging solution for the Slide Kick project, providing consistent logging across:

- ESP32 main controller firmware
- RP2040 subsystem firmware
- sk-slide motorized fader library
- sk-oal OSC abstraction layer
- Desktop tools and applications

## License

MIT License - see [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please ensure:

1. Code follows existing style conventions
2. All platforms remain supported
3. Examples compile and run correctly
4. Documentation is updated for new features

