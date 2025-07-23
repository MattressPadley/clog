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
5. **Compile-time Filtering**: Log levels can be filtered out at compile time for performance
6. **Fixed Buffer Sizes**: Uses fixed-size buffers (default 512 bytes) for embedded compatibility

### Log Levels
- `OFF` (0) - No logging
- `ERROR` (1) - Error messages only  
- `WARN` (2) - Warnings and errors
- `INFO` (3) - Informational messages and above (default)
- `DEBUG` (4) - Debug messages and above
- `TRACE` (5) - All messages including trace

### Tag-Based Filtering

CLog supports granular filtering by tag names in addition to log levels, allowing you to focus on specific components or functional areas of your application.

#### Filtering Modes

- **ALLOW_ALL** (default): No tag filtering, all tags are logged
- **WHITELIST**: Only explicitly enabled tags are logged
- **BLACKLIST**: All tags are logged except explicitly disabled ones

#### Tag Management API

```cpp
// Enable specific tags (switches to whitelist mode)
clogger::Logger::enableTag("Database");
clogger::Logger::enableTag("Network");

// Disable specific tags (switches to blacklist mode)
clogger::Logger::disableTag("Debug");
clogger::Logger::disableTag("Verbose");

// Bulk operations
clogger::Logger::enableAllTags();   // Allow all tags (default)
clogger::Logger::disableAllTags();  // Block all tags (empty whitelist)

// Query tag status
bool isEnabled = clogger::Logger::isTagEnabled("MyTag");

// Clear all filters
clogger::Logger::clearTagFilters();
```

#### Integration with Log Levels

Tag filtering works in combination with log level filtering:
1. First, the log level is checked (e.g., DEBUG > INFO gets filtered)
2. Then, if the level passes, the tag filter is checked
3. Only messages that pass both filters are logged

#### Use Cases

- **Development**: Enable only tags for the feature you're working on
- **Debugging**: Disable noisy tags while keeping relevant ones
- **Production**: Whitelist only critical system tags
- **Component Testing**: Focus on specific subsystem logs

### Usage Patterns

**Standalone logging:**
```cpp
#include <clog/log.hpp>
clogger::Logger::setLevel(clogger::Level::DEBUG);
CLOG_INFO("MyApp", "Application started");
```

**Callback integration for parent applications:**
```cpp
void logCallback(clogger::Level level, const char* tag, const char* message) {
    // Route to existing logging system
}
clogger::Logger::setCallback(logCallback);
```

**Tag-based filtering for granular control:**
```cpp
// Enable only specific tags (whitelist mode)
clogger::Logger::enableTag("Database");
clogger::Logger::enableTag("Network");
CLOG_INFO("Database", "Will appear");    // ✓ Logged
CLOG_INFO("UI", "Will be filtered");     // ✗ Filtered out

// Disable specific tags (blacklist mode)  
clogger::Logger::enableAllTags();
clogger::Logger::disableTag("Debug");
CLOG_INFO("App", "Will appear");         // ✓ Logged
CLOG_INFO("Debug", "Will be filtered");  // ✗ Filtered out

// Check tag status programmatically
bool enabled = clogger::Logger::isTagEnabled("Database");
```

### Build Integration

- **CMake**: Uses `CMakeLists.txt` with interface library target `clogger::clog`
- **PlatformIO**: Uses `library.json` for embedded platforms  
- **Arduino IDE**: Header-only, just include `<clog/log.hpp>`
- **Just**: Uses `justfile` for development commands

### Configuration

Compile-time configuration via preprocessor defines:
- `CLOG_LEVEL` - Set maximum log level (0-5)
- `CLOG_BUFFER_SIZE` - Message buffer size (default: 256 embedded, 512 desktop)
- `CLOG_ENABLE_COLORS` - Enable color output (default: desktop only)
- `CLOG_MAX_TAG_FILTERS` - Maximum number of tag filters (default: 16)
- `CLOG_ENABLE_TAG_FILTERING` - Enable/disable tag filtering feature (default: enabled)

### Testing Structure

- **Unit Tests**: `tests/unit/` - Test core logging functionality
- **Integration Tests**: `tests/integration/` - Test platform-specific behavior
- **Examples**: `examples/` - Demonstrate usage on different platforms

### Platform Support

- **Desktop**: Windows, Linux, macOS with colored console output
- **Arduino**: All Arduino-compatible boards with Serial output
- **ESP32/ESP8266**: Both Arduino framework and ESP-IDF
- **RP2040**: Arduino framework and Pico SDK support

The library is designed as a unified logging solution for embedded and desktop C++ applications, with particular focus on the Slide Kick project ecosystem.