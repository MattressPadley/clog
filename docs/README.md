# CLog - Unified Logging Library

CLog is a lightweight, header-only C++ logging library designed for cross-platform compatibility across embedded systems (Arduino, ESP32, RP2040) and desktop platforms. It provides consistent logging APIs with callback support for integration into larger applications.

## Features

- **Header-only**: Single include, no compilation required
- **Cross-platform**: Works on Arduino, ESP32, RP2040, and desktop systems
- **Lightweight**: Minimal overhead suitable for embedded systems
- **Flexible**: Supports both direct output and callback-based integration
- **Dual filtering**: Both log level and tag-based filtering for granular control
- **Configurable**: Compile-time log level filtering and buffer size configuration
- **Colorized output**: Level-based colors and configurable tag colors for better visual distinction
- **Zero dependencies**: Only uses standard C++ library features

## Quick Start

### Basic Usage

```cpp
#include <clog/log.hpp>

int main() {
    // Set log level
    clogger::Logger::setLevel(clogger::Level::DEBUG);
    
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

void myLogCallback(clogger::Level level, const char* tag, const char* message) {
    // Route CLog messages to your existing logging system
    printf("[%s] %s: %s\n", levelToString(level), tag, message);
}

int main() {
    // Register callback to capture all library logging
    clogger::Logger::setCallback(myLogCallback);
    clogger::Logger::setLevel(clogger::Level::DEBUG);
    
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
target_link_libraries(your_target clogger::clog)
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
#define CLOG_LEVEL 4              // 0=OFF, 1=ERROR, 2=WARN, 3=INFO, 4=DEBUG, 5=TRACE
#define CLOG_BUFFER_SIZE 512      // Message buffer size
#define CLOG_MAX_TAG_FILTERS 16   // Maximum number of tag filters
#define CLOG_ENABLE_TAG_FILTERING 1 // Enable tag filtering (default: enabled)
#include <clog/log.hpp>
```

### Build System Configuration

**CMake:**
```cmake
target_compile_definitions(your_target PRIVATE 
    CLOG_LEVEL=4
    CLOG_BUFFER_SIZE=256
    CLOG_MAX_TAG_FILTERS=16
    CLOG_ENABLE_TAG_FILTERING=1
)
```

**PlatformIO:**
```ini
build_flags = 
    -DCLOG_LEVEL=4
    -DCLOG_BUFFER_SIZE=256
    -DCLOG_MAX_TAG_FILTERS=16
    -DCLOG_ENABLE_TAG_FILTERING=1
```

## API Reference

### Log Levels

- `clogger::Level::OFF` - No logging
- `clogger::Level::ERROR` - Error messages only
- `clogger::Level::WARN` - Warnings and errors
- `clogger::Level::INFO` - Informational messages and above
- `clogger::Level::DEBUG` - Debug messages and above
- `clogger::Level::TRACE` - All messages including trace

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
clogger::Logger::setLevel(clogger::Level::DEBUG);

// Get current log level
clogger::Level level = clogger::Logger::getLevel();

// Set callback for message handling
clogger::Logger::setCallback(myCallback);

// Enable/disable direct output
clogger::Logger::enableDirectOutput(true);

// Platform-specific initialization (usually not needed)
clogger::Logger::init();
```

### Tag Filtering

CLog supports granular filtering by tag names, allowing you to focus on specific components or functional areas of your application. This works in combination with log level filtering.

#### Tag Management Methods

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

#### Tag Filtering Modes

- **ALLOW_ALL** (default): No tag filtering, all tags are logged
- **WHITELIST**: Only explicitly enabled tags are logged
- **BLACKLIST**: All tags are logged except explicitly disabled ones

#### Example Usage

```cpp
#include <clog/log.hpp>

int main() {
    clogger::Logger::setLevel(clogger::Level::DEBUG);
    
    // Example 1: Whitelist mode - only show Database and Security logs
    clogger::Logger::enableTag("Database");
    clogger::Logger::enableTag("Security");
    
    CLOG_INFO("Database", "Connection established");    // ✓ Shown
    CLOG_INFO("Network", "Packet received");           // ✗ Filtered out
    CLOG_INFO("Security", "User authenticated");       // ✓ Shown
    
    // Example 2: Blacklist mode - show all except Debug logs
    clogger::Logger::enableAllTags();
    clogger::Logger::disableTag("Debug");
    
    CLOG_INFO("App", "Application started");           // ✓ Shown
    CLOG_INFO("Debug", "Verbose debug info");          // ✗ Filtered out
    
    // Example 3: Check tag status programmatically
    if (clogger::Logger::isTagEnabled("Performance")) {
        CLOG_INFO("Performance", "CPU usage: 45%");
    }
    
    return 0;
}
```

#### Integration with Log Levels

Tag filtering and log level filtering work together:
1. First, the log level is checked (e.g., DEBUG > INFO gets filtered)
2. Then, if the level passes, the tag filter is checked
3. Only messages that pass both filters are logged

This allows for fine-grained control: you can set a high log level (like DEBUG) but only enable specific tags you're interested in.

### Tag Color Configuration

CLog supports configurable colors for tags on desktop platforms, making it easier to visually distinguish between different system components in log output.

#### Available Colors

```cpp
enum class Color {
    DEFAULT,        // No color (system default)
    BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE,
    BRIGHT_BLACK, BRIGHT_RED, BRIGHT_GREEN, BRIGHT_YELLOW,
    BRIGHT_BLUE, BRIGHT_MAGENTA, BRIGHT_CYAN, BRIGHT_WHITE
};
```

#### Tag Color Methods

```cpp
// Set a custom color for a specific tag
clogger::Logger::setTagColor("Database", clogger::Color::BRIGHT_CYAN);
clogger::Logger::setTagColor("Network", clogger::Color::BRIGHT_MAGENTA);
clogger::Logger::setTagColor("Security", clogger::Color::BRIGHT_RED);

// Remove custom color from a tag (returns to default)
clogger::Logger::clearTagColor("Database");

// Clear all tag colors
clogger::Logger::clearAllTagColors();
```

#### Tag Color Usage Example

```cpp
#include <clog/log.hpp>

int main() {
    // Configure colors for different system components
    clogger::Logger::setTagColor("Database", clogger::Color::BRIGHT_CYAN);
    clogger::Logger::setTagColor("Network", clogger::Color::BRIGHT_MAGENTA);
    clogger::Logger::setTagColor("Security", clogger::Color::BRIGHT_RED);
    clogger::Logger::setTagColor("UI", clogger::Color::BRIGHT_GREEN);
    
    // Now these tags will appear in their configured colors
    CLOG_INFO("Database", "Connection established");
    CLOG_WARN("Network", "High latency detected");
    CLOG_ERROR("Security", "Authentication failed");
    CLOG_DEBUG("UI", "Window rendered successfully");
    CLOG_INFO("DefaultTag", "This appears in default color");
    
    return 0;
}
```

#### Output Format

The colored output format is: `[<colored_level>] <colored_tag>: message`

- **Level text** is colored based on log level (ERROR=red, WARN=yellow, INFO=green, etc.)
- **Tag text** is colored based on your configuration (or default if not configured)
- **Everything else** (brackets, colon, message) remains in default color

#### Platform Support

- **Desktop platforms**: Full color support with ANSI escape codes
- **Embedded platforms**: Tag colors are ignored to save memory; only level colors are used
- **Arduino/ESP32**: Colors are not displayed but tag color calls are safely ignored

#### Limitations

- Maximum of 32 different tag colors can be configured simultaneously
- Tag names are limited to 31 characters
- Colors only affect direct console output (not callback-based logging)

## Examples

The library includes comprehensive examples:

- **Desktop Example**: Basic usage with colored console output, tag color configuration, and tag filtering demonstration
- **Arduino Example**: Embedded system logging with sensor simulation
- **ESP32 Advanced**: Multi-task logging with WiFi and web server integration
- **Callback Integration**: Advanced integration with parent application logging systems

### Running Examples

```bash
# Run desktop example with tag color and filtering showcase
just run-desktop-example

# Build and run other examples
just build-examples
```

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

