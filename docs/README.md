# CLog - Unified Logging Library

CLog is a lightweight, header-only C++ logging library designed for cross-platform compatibility across embedded systems (Arduino, ESP32, RP2040) and desktop platforms.

## Features

- **Header-only**: Single include, no compilation required
- **Cross-platform**: Works on Arduino, ESP32, RP2040, and desktop systems  
- **Lightweight**: Minimal overhead suitable for embedded systems
- **Flexible**: Supports both direct output and callback-based integration
- **Dual filtering**: Both log level and tag-based filtering for granular control
- **Library tagging**: Automatic identification for nested dependencies
- **Configurable**: Compile-time log level filtering and buffer size configuration
- **Colorized output**: Level-based colors and configurable tag colors
- **Zero dependencies**: Only uses standard C++ library features

## Quick Start

```cpp
#define CLOG_LEVEL 4  // DEBUG level
#include <clog/log.hpp>

int main() {
    CLOG_INFO("MyApp", "Application started");
    CLOG_DEBUG("Network", "Connected to server");
    return 0;
}
```

**CMake Integration:**
```cmake
add_subdirectory(external/clog)
target_compile_definitions(my_app PRIVATE CLOG_LEVEL=4)
target_link_libraries(my_app PRIVATE clog::clog)
```

## Documentation

### Core Guides
- **[Quick Start](quick-start.md)** - Get up and running in minutes
- **[Installation](installation.md)** - Installation methods and CMake integration
- **[Configuration](configuration.md)** - Complete compile-time configuration guide
- **[API Reference](api-reference.md)** - Complete API documentation

### Advanced Topics  
- **[Platform Support](platform-support.md)** - Platform-specific setup and features
- **[Multi-Library Projects](multi-library-projects.md)** - Log level hierarchy and best practices
- **[Performance](performance.md)** - Optimization strategies and benchmarks

### Overview
- **[Overview](overview.md)** - Detailed introduction and architecture

## Platform Support

| Platform | Status | Features |
|----------|--------|----------|
| **Desktop** (Windows/Linux/macOS) | ✅ Full | Colors, all features |
| **Arduino** (All boards) | ✅ Full | Serial output, optimized |
| **ESP32/ESP8266** | ✅ Full | Arduino + ESP-IDF support |
| **RP2040** | ✅ Full | Arduino + Pico SDK support |

## Key Concepts

### Log Levels
- `CLOG_ERROR` - Error messages (level 1)
- `CLOG_WARN` - Warning messages (level 2)  
- `CLOG_INFO` - Informational messages (level 3)
- `CLOG_DEBUG` - Debug messages (level 4)
- `CLOG_TRACE` - Trace messages (level 5)

### Compile-time Filtering
```cpp
#define CLOG_LEVEL 2  // Only ERROR and WARN levels
#include <clog/log.hpp>

CLOG_ERROR("App", "This will be compiled");    // ✓ Included
CLOG_INFO("App", "This will be eliminated");   // ✗ Zero code generated
```

### Multi-Library Support
```cmake
# Library sets reasonable default
target_compile_definitions(my_lib PRIVATE CLOG_LEVEL=3)

# Application overrides for all libraries  
target_compile_definitions(my_app PRIVATE CLOG_LEVEL=5)
```

## Examples

Check the `examples/` directory for complete, runnable examples:
- **Desktop**: Full-featured desktop logging with colors
- **Arduino**: Basic Arduino sketch with serial output
- **ESP32**: Advanced multi-task logging with WiFi
- **Callback**: Integration with existing logging systems

## Integration with Slide Kick

CLog serves as the unified logging solution for the Slide Kick project ecosystem:
- ESP32 main controller firmware
- RP2040 subsystem firmware  
- sk-slide motorized fader library
- sk-oal OSC abstraction layer
- Desktop tools and applications

## Contributing

Contributions are welcome! Please ensure:
1. Code follows existing style conventions
2. All platforms remain supported
3. Examples compile and run correctly
4. Documentation is updated for new features

## License

MIT License - see [LICENSE](../LICENSE) file for details.

---

## Need Help?

- **[Quick Start Guide](quick-start.md)** - New to CLog? Start here
- **[Installation Guide](installation.md)** - Having trouble with setup?
- **[Configuration Guide](configuration.md)** - Need to customize CLog?
- **[API Reference](api-reference.md)** - Looking for specific functions?
- **[Multi-Library Guide](multi-library-projects.md)** - Working with multiple libraries?
- **[Platform Guide](platform-support.md)** - Platform-specific questions?
- **[Performance Guide](performance.md)** - Optimization and benchmarks