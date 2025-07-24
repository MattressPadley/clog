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

### Platform Configuration

CLog supports explicit platform configuration to avoid compile-time detection issues:

**Available Platforms:**
- `Platform::ARDUINO` - Arduino framework
- `Platform::ESP32` - ESP32 boards
- `Platform::ESP8266` - ESP8266 boards  
- `Platform::RP2040_ARDUINO` - RP2040 with Arduino framework
- `Platform::RP2040_SDK` - RP2040 with Pico SDK
- `Platform::ESP_IDF` - ESP-IDF framework
- `Platform::DESKTOP` - Generic desktop platform
- `Platform::WINDOWS` - Windows desktop
- `Platform::LINUX` - Linux desktop
- `Platform::MACOS` - macOS desktop
- `Platform::AUTO_DETECT` - Automatic detection (may trigger warnings)

**Configuration API:**
```cpp
// Default is DESKTOP platform (works for most development)
// For embedded platforms, set explicitly:
clogger::Logger::init(clogger::Platform::RP2040_SDK);

// Or set platform separately
clogger::Logger::setPlatform(clogger::Platform::RP2040_SDK);
clogger::Logger::init();

// Query current platform
clogger::Platform current = clogger::Logger::getPlatform();
```

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

### Library Tagging System

CLog supports automatic library identification for nested dependency scenarios, allowing parent applications to distinguish logs from different libraries while giving libraries full autonomy over their own identification.

#### How It Works

1. **Libraries Self-Identify**: Each library calls `setLibraryName()` once during initialization
2. **Parent Controls Visibility**: Parent applications use `enableLibraryTags()` to control whether library names appear
3. **Independent Operation**: Libraries work identically whether used standalone or as dependencies

#### Library Tagging API

```cpp
// Library identification (called by libraries during init)
clogger::Logger::setLibraryName("MyLibraryName");
const char* name = clogger::Logger::getLibraryName();

// Visibility control (called by parent applications)
clogger::Logger::enableLibraryTags(true);   // Show library names
clogger::Logger::enableLibraryTags(false);  // Hide library names
bool enabled = clogger::Logger::isLibraryTagsEnabled();

// Library-specific colors
clogger::Logger::setLibraryColor("Database", clogger::Color::BRIGHT_CYAN);
clogger::Logger::clearLibraryColor("Database");
clogger::Logger::clearAllLibraryColors();
```

#### Output Format

- **Library tags disabled** (default): `[ERROR] [Tag]: message`
- **Library tags enabled**: `[ERROR] [LibraryName][Tag]: message`
- **With colors**: `[ERROR] [cyan_Library][green_Tag]: message`

#### Use Cases

- **Library Development**: Set library name but keep tags disabled for clean development output
- **Integration Testing**: Enable library tags to see which library each log comes from
- **Production Debugging**: Quickly identify problematic libraries in multi-library applications
- **Component Isolation**: Filter logs by library for focused debugging

#### Integration Workflow

```cpp
// In MyDatabaseLib/src/database.cpp
void initDatabase() {
    clogger::Logger::setLibraryName("DatabaseLib");
    CLOG_INFO("Init", "Database library initialized");  // [Init]: message
}

// In parent application main.cpp
int main() {
    initDatabase();
    
    // Enable library identification
    clogger::Logger::enableLibraryTags(true);
    clogger::Logger::setLibraryColor("DatabaseLib", clogger::Color::BRIGHT_BLUE);
    
    // Now all DatabaseLib logs show: [DatabaseLib][Tag]: message
    performDatabaseOperation(); // [DatabaseLib][Query]: SELECT completed
    
    return 0;
}
```

### Usage Patterns

**Platform configuration (recommended for RP2040 and other platforms):**
```cpp
#include <clog/log.hpp>

// Explicit platform configuration - eliminates build warnings
clogger::Logger::init(clogger::Platform::RP2040_SDK);
// or
clogger::Logger::setPlatform(clogger::Platform::RP2040_SDK);
clogger::Logger::init();

CLOG_INFO("MyApp", "Application started");
```

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

**Library tagging for nested dependencies:**
```cpp
// In library code (during initialization)
clogger::Logger::setLibraryName("MyLib");

// In parent application - control library tag visibility
clogger::Logger::enableLibraryTags(true);   // Show [MyLib][Tag]: message
clogger::Logger::enableLibraryTags(false);  // Show [Tag]: message

// Configure colors for libraries and tags independently
clogger::Logger::setLibraryColor("MyLib", clogger::Color::BRIGHT_CYAN);
clogger::Logger::setTagColor("Database", clogger::Color::BRIGHT_GREEN);

// Results in colored output: [cyan_MyLib][green_Database]: message
CLOG_INFO("Database", "Connection established");
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
- `CLOG_MAX_LIBRARY_COLORS` - Maximum number of library colors (default: 16)
- `CLOG_MAX_LIBRARY_NAME_LENGTH` - Maximum library name length (default: 32)
- `CLOG_ENABLE_ASSERTS` - Enable assertion macros (default: enabled in debug builds)

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