# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

### Development Build
- `just build` - Build library and examples using CMake
- `just build-examples` - Build examples only
- `just build-tests` - Build test suite

### Testing
- `just test` - Run all tests (unit and integration)
- `just test-unit` - Run unit tests only
- `just test-integration` - Run integration tests only
- `just test-specific <test_name>` - Run specific test by name
- `just test-debug` - Run tests with detailed output
- `scripts/run_tests.sh` - Comprehensive test runner with detailed reporting

### Examples
- `just run-desktop-example` - Run desktop logging example
- `just run-callback-example` - Run callback integration example

### Maintenance
- `just clean` - Clean all build artifacts
- `just format` - Format code using clang-format (if available)
- `just analyze` - Run static analysis with cppcheck (if available)
- `just validate` - Validate project structure

### CI/Quality Checks
- `just ci` - Run CI-like checks (build + test + format check)
- `just pre-release` - Run all pre-release validation checks
- `just full-check` - Complete validation including benchmarks

## Code Architecture

CLog is a **header-only C++ logging library** designed for cross-platform compatibility between embedded systems (Arduino, ESP32, RP2040) and desktop platforms.

### Core Components

- **`include/clog/log.hpp`** - Main header containing complete logging implementation
- **`include/clog/platform.hpp`** - Platform detection and abstraction utilities  
- **`include/clog/config.hpp`** - Compile-time configuration options

### Key Architecture Patterns

1. **Single Header Design**: Complete implementation in `log.hpp` for easy integration
2. **Platform Abstraction**: Automatic detection of Arduino, ESP32, RP2040, and desktop platforms
3. **Callback Integration**: Support for routing logs to parent application logging systems
4. **Dual Filtering System**: Both log level and tag-based filtering for granular control
5. **Library Tagging System**: Automatic nested library identification with parent-controlled visibility
6. **Compile-time Filtering**: Log levels can be filtered out at compile time for performance
7. **Fixed Buffer Sizes**: Uses fixed-size buffers (default 512 bytes) for embedded compatibility

### Log Levels
- `OFF` (0) - No logging
- `ERROR` (1) - Error messages only  
- `WARN` (2) - Warnings and errors
- `INFO` (3) - Informational messages and above (default)
- `DEBUG` (4) - Debug messages and above
- `TRACE` (5) - All messages including trace

### Common Usage Patterns

**Basic logging:**
```cpp
#include <clog/log.hpp>
CLOG_INFO("MyApp", "Application started");
```

**CMake integration:**
```cmake
find_package(clog QUIET)
if(NOT clog_FOUND)
    add_subdirectory(path/to/clog)
endif()
target_link_libraries(your_target clog::clog)
```

**Library identification:**
```cmake
target_compile_definitions(my_lib PRIVATE CLOG_LIBRARY_NAME="MyLib")
```

### Build Integration

- **CMake**: Uses `CMakeLists.txt` with interface library target `clog::clog`
- **PlatformIO**: Uses `library.json` for embedded platforms  
- **Arduino IDE**: Header-only, just include `<clog/log.hpp>`
- **Just**: Uses `justfile` for development commands

### Testing Structure

- **Unit Tests**: `tests/unit/` - Test core logging functionality
- **Integration Tests**: `tests/integration/` - Test platform-specific behavior
- **Examples**: `examples/` - Demonstrate usage on different platforms

### Platform Support

- **Desktop**: Windows, Linux, macOS with colored console output
- **Arduino**: All Arduino-compatible boards with Serial output
- **ESP32/ESP8266**: Both Arduino framework and ESP-IDF
- **RP2040**: Arduino framework and Pico SDK support

## Documentation

For detailed information beyond this quick reference, see the comprehensive documentation in `docs/`:

- **[Overview](docs/overview.md)** - Complete project overview, features, and design philosophy
- **[Quick Start](docs/quick-start.md)** - Getting started guide with examples
- **[Installation](docs/installation.md)** - Installation instructions for all platforms
- **[Configuration](docs/configuration.md)** - Complete configuration guide with all options
- **[Platform Support](docs/platform-support.md)** - Detailed platform-specific information
- **[Multi-Library Projects](docs/multi-library-projects.md)** - Best practices for library hierarchy and log level management
- **[API Reference](docs/api-reference.md)** - Complete API documentation
- **[Performance](docs/performance.md)** - Performance characteristics and optimization

The library is designed as a unified logging solution for embedded and desktop C++ applications, with particular focus on the Slide Kick project ecosystem.