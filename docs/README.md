 CLog - Unified Logging Library

CLog is a lightweight, header-only C++ logging library designed for cross-platform compatibility across embedded systems (Arduino, ESP32, RP2040) and desktop platforms. It provides consistent logging APIs with callback support for integration into larger applications.

## Features

- **Header-only**: Single include, no compilation required
- **Cross-platform**: Works on Arduino, ESP32, RP2040, and desktop systems
- **Lightweight**: Minimal overhead suitable for embedded systems
- **Flexible**: Supports both direct output and callback-based integration
- **Dual filtering**: Both log level and tag-based filtering for granular control
- **Library tagging**: Automatic identification for nested dependencies with parent-controlled visibility
- **Configurable**: Compile-time log level filtering and buffer size configuration
- **Colorized output**: Level-based colors and configurable tag colors for better visual distinction
- **Zero dependencies**: Only uses standard C++ library features

## Quick Start

### Basic Usage

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

### CMake Configuration (Preferred)

```cmake
# Configure via CMake for better build system integration
add_executable(my_app main.cpp)
target_compile_definitions(my_app PRIVATE 
    CLOG_LEVEL=4                    # Debug level
    CLOG_PLATFORM=DESKTOP           # Explicit platform (optional)
)
target_link_libraries(my_app PRIVATE clog::clog)
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
    // Note: Log level configured at compile time via CLOG_LEVEL
    
    // All CLOG_* calls will now go through your callback
    CLOG_INFO("System", "Library integrated successfully");
    
    return 0;
}
```

## Platform Configuration

CLog uses simplified platform detection that defaults to desktop behavior and can be overridden via CMake for embedded platforms. This approach eliminates complex auto-detection and provides reliable cross-platform operation.

### Supported Platforms

- **Desktop**: Windows, Linux, macOS (default behavior)
- **Arduino**: All Arduino-compatible boards
- **ESP32/ESP8266**: Both Arduino framework and ESP-IDF
- **Raspberry Pi Pico**: RP2040 with Arduino framework or Pico SDK

### CMake Platform Configuration (Recommended)

```cmake
# Set platform at build time via CMake
set(CLOG_PLATFORM "RP2040_SDK" CACHE STRING "CLog target platform")

# Or pass as command-line argument
# cmake -DCLOG_PLATFORM=ESP32 ..

# Available platform values:
# AUTO_DETECT (default), ARDUINO, ESP32, ESP8266, 
# RP2040_ARDUINO, RP2040_SDK, ESP_IDF,
# DESKTOP, WINDOWS, LINUX, MACOS
```

### Runtime Platform Configuration (Alternative)

```cpp
// Available platforms (for runtime configuration)
enum class Platform {
    ARDUINO, ESP32, ESP8266, RP2040_ARDUINO, RP2040_SDK,
    ESP_IDF, DESKTOP, WINDOWS, LINUX, MACOS, AUTO_DETECT
};

// Explicit platform configuration (when CMake config not available)
clogger::Logger::init(clogger::Platform::RP2040_SDK);

// Or set platform separately
clogger::Logger::setPlatform(clogger::Platform::RP2040_SDK);
clogger::Logger::init();

// Query current platform
clogger::Platform current = clogger::Logger::getPlatform();
```

### Platform-Specific Usage

**RP2040 with Pico SDK (CMake approach):**
```cmake
# In CMakeLists.txt
add_executable(pico_app main.cpp)
target_compile_definitions(pico_app PRIVATE 
    CLOG_LEVEL=4
    CLOG_PLATFORM_RP2040_SDK=1
    CLOG_HAS_COLOR_SUPPORT=0
    CLOG_HAS_PRINTF_SUPPORT=1
)
target_link_libraries(pico_app PRIVATE clog::clog pico_stdlib)
```

```cpp
#include <clog/log.hpp>

int main() {
    // No platform configuration needed - configured via CMake
    CLOG_INFO("App", "RP2040 application started");
    return 0;
}
```

**Desktop Development (default):**
```cpp
// Desktop defaults: colors enabled, std::cout output
#include <clog/log.hpp>

int main() {
    // No configuration needed - defaults to desktop behavior
    CLOG_INFO("App", "Desktop application started");
    return 0;
}
```

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

### Nested Dependencies

When using CLog as a submodule in multiple nested projects (e.g., a library that uses CLog is itself used by an application that also uses CLog), use the following pattern to avoid duplicate target definitions:

```cmake
# Recommended pattern for nested submodule usage
find_package(clog QUIET)
if(NOT clog_FOUND)
    add_subdirectory(external/clog)
endif()

target_link_libraries(your_target clog::clog)
```

**Example Scenario:**
- Your main application includes CLog as a submodule
- Your application also uses MyDatabaseLib, which includes CLog as a submodule  
- Both projects use the `find_package()` fallback pattern

**What happens:**
1. First project to configure finds `clog_FOUND=false`, runs `add_subdirectory()`
2. Second project finds `clog_FOUND=true`, skips `add_subdirectory()`
3. Both projects link to the same `clog::clog` target without conflicts

**Benefits:**
- No duplicate target definition errors
- Automatic deduplication through CMake's export/import system
- Works with any level of nesting
- Compatible with existing CMake workflows

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

CLog is designed for compile-time configuration to achieve optimal performance. The recommended approach is to configure via CMake, with preprocessor defines as an alternative.

### CMake Configuration (Recommended)

```cmake
# Complete configuration example
add_executable(my_app main.cpp)
target_compile_definitions(my_app PRIVATE 
    # Core configuration
    CLOG_LEVEL=4                        # 0=OFF, 1=ERROR, 2=WARN, 3=INFO, 4=DEBUG, 5=TRACE
    CLOG_BUFFER_SIZE=512               # Message buffer size
    
    # Platform configuration (optional - defaults to desktop)
    CLOG_PLATFORM_DESKTOP=1            # Explicit desktop platform
    CLOG_HAS_COLOR_SUPPORT=1           # Enable colored output
    CLOG_HAS_PRINTF_SUPPORT=0          # Use std::cout (not printf)
    
    # Feature configuration
    CLOG_ENABLE_TAG_FILTERING=1        # Enable tag filtering (default: enabled)
    CLOG_MAX_TAG_FILTERS=16            # Maximum number of tag filters
    CLOG_MAX_LIBRARY_COLORS=16         # Maximum number of library colors
    CLOG_MAX_LIBRARY_NAME_LENGTH=32    # Maximum library name length
    CLOG_ENABLE_ASSERTS=1              # Enable assertion macros
)
target_link_libraries(my_app PRIVATE clog::clog)
```

### Preprocessor Configuration (Alternative)

```cpp
// Define before including clog/log.hpp
#define CLOG_LEVEL 4              // DEBUG level
#define CLOG_BUFFER_SIZE 512      // Message buffer size
#define CLOG_MAX_TAG_FILTERS 16   // Maximum number of tag filters
#define CLOG_ENABLE_TAG_FILTERING 1 // Enable tag filtering
#include <clog/log.hpp>
```

### PlatformIO Configuration

```ini
[env:my_env]
build_flags = 
    -DCLOG_LEVEL=4
    -DCLOG_BUFFER_SIZE=256
    -DCLOG_PLATFORM_ESP32=1
    -DCLOG_HAS_COLOR_SUPPORT=0
    -DCLOG_HAS_PRINTF_SUPPORT=1
```

### Available Configuration Options

**Core Settings:**
- `CLOG_LEVEL` - Maximum log level (0-5), messages above this are compiled out
- `CLOG_BUFFER_SIZE` - Buffer size for formatted messages
- `CLOG_ENABLE_ASSERTS` - Enable/disable assertion macros

**Platform Settings:**
- `CLOG_PLATFORM_*` - Explicit platform selection (see platform section)
- `CLOG_HAS_COLOR_SUPPORT` - Enable colored console output
- `CLOG_HAS_PRINTF_SUPPORT` - Use printf vs std::cout for output

**Feature Settings:**
- `CLOG_ENABLE_TAG_FILTERING` - Enable tag-based filtering system
- `CLOG_MAX_TAG_FILTERS` - Maximum number of simultaneous tag filters
- `CLOG_MAX_LIBRARY_COLORS` - Maximum library color configurations
- `CLOG_MAX_LIBRARY_NAME_LENGTH` - Maximum length for library names

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
// Log level configuration (compile-time preferred)
// Note: setLevel() still available for runtime adjustment, but compile-time filtering
// via CLOG_LEVEL provides better performance by eliminating unused code
clogger::Logger::setLevel(clogger::Level::DEBUG);  // Runtime level (if needed)
clogger::Level level = clogger::Logger::getLevel(); // Get current runtime level

// Callback integration
clogger::Logger::setCallback(myCallback);     // Set callback for message handling
clogger::Logger::enableDirectOutput(true);   // Enable/disable direct console output

// Platform configuration (optional - defaults to desktop)
clogger::Logger::init();                           // Use default platform detection
clogger::Logger::init(clogger::Platform::RP2040_SDK); // Explicit platform
clogger::Logger::setPlatform(clogger::Platform::ESP32); // Set platform separately

// Platform queries
clogger::Platform current = clogger::Logger::getPlatform();
bool isDesktop = clogger::Logger::isDesktopPlatform();
bool isEmbedded = clogger::Logger::isEmbeddedPlatform();
bool hasColors = clogger::Logger::hasColorSupport();
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
// Configure compile-time log level
#define CLOG_LEVEL 4  // DEBUG level
#include <clog/log.hpp>

int main() {
    // No setLevel() call needed - configured at compile time
    
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
    CLOG_TRACE("Network", "Detailed trace");           // ✗ May be compiled out if CLOG_LEVEL < 5
    
    // Example 3: Check tag status programmatically
    if (clogger::Logger::isTagEnabled("Performance")) {
        CLOG_INFO("Performance", "CPU usage: 45%");
    }
    
    return 0;
}
```

#### Integration with Log Levels

Tag filtering and log level filtering work together with a two-stage approach:

**Compile-time filtering (CLOG_LEVEL):**
1. Messages above the compile-time level are completely eliminated from the binary
2. No runtime overhead for filtered-out levels

**Runtime filtering (tag filtering + runtime level):**
1. For messages that survive compile-time filtering, runtime level is checked
2. Then tag filters are applied
3. Only messages passing both checks are logged

**Example:**
```cpp
#define CLOG_LEVEL 4  // Compile out TRACE (level 5)
// ... 
clogger::Logger::setLevel(clogger::Level::WARN);  // Runtime level
clogger::Logger::enableTag("Database");

CLOG_ERROR("Database", "Error");     // ✓ Shown (compiled, level passes, tag enabled)
CLOG_INFO("Database", "Info");       // ✗ Hidden (compiled, level filtered at runtime)
CLOG_DEBUG("Network", "Debug");      // ✗ Hidden (compiled, level + tag filtered)
CLOG_TRACE("Database", "Trace");     // ✗ Not compiled (eliminated at compile time)
```

This provides optimal performance through compile-time elimination while maintaining runtime flexibility.

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

### Library Tagging for Nested Dependencies

CLog provides automatic library identification for scenarios where your logging library is used as a dependency in other projects. This allows parent applications to distinguish logs from different libraries while keeping libraries completely autonomous.

#### How Library Tagging Works

1. **Libraries Self-Identify**: Each library calls `setLibraryName()` once during initialization
2. **Parent Controls Visibility**: Parent applications use `enableLibraryTags()` to control whether library names appear in logs
3. **Independent Operation**: Libraries work identically whether used standalone or as dependencies

#### Library Tagging Methods

```cpp
// Library identification (called by libraries during initialization)
clogger::Logger::setLibraryName("MyLibraryName");
const char* name = clogger::Logger::getLibraryName();

// Visibility control (called by parent applications)
clogger::Logger::enableLibraryTags(true);   // Show library names in logs
clogger::Logger::enableLibraryTags(false);  // Hide library names (default)
bool enabled = clogger::Logger::isLibraryTagsEnabled();

// Library-specific colors (independent from tag colors)
clogger::Logger::setLibraryColor("DatabaseLib", clogger::Color::BRIGHT_CYAN);
clogger::Logger::clearLibraryColor("DatabaseLib");
clogger::Logger::clearAllLibraryColors();
```

#### Output Format with Library Tagging

- **Library tags disabled** (default): `[ERROR] [Tag]: message`
- **Library tags enabled**: `[ERROR] [LibraryName][Tag]: message`
- **With colors**: `[ERROR] [cyan_LibraryName][green_Tag]: message`

#### Usage Example

**In your library code (MyDatabaseLib/src/database.cpp):**
```cpp
#include <clog/log.hpp>

void initDatabase() {
    // Library identifies itself during initialization
    clogger::Logger::setLibraryName("DatabaseLib");
    
    // Normal logging - no changes needed to existing code
    CLOG_INFO("Init", "Database library initialized");  // Shows: [Init]: message
    CLOG_INFO("Connection", "Connected to server");      // Shows: [Connection]: message
}

void performQuery(const std::string& sql) {
    CLOG_DEBUG("Query", "Executing: %s", sql.c_str());
    // ... database logic ...
    CLOG_INFO("Query", "Query completed successfully");
}
```

**In parent application (main.cpp):**
```cpp
#include <clog/log.hpp>
#include "MyDatabaseLib/database.h"

int main() {
    // Initialize library (library sets its own name)
    initDatabase();
    
    // Enable library identification in logs
    clogger::Logger::enableLibraryTags(true);
    
    // Configure colors for better visual distinction
    clogger::Logger::setLibraryColor("DatabaseLib", clogger::Color::BRIGHT_BLUE);
    clogger::Logger::setTagColor("Query", clogger::Color::BRIGHT_GREEN);
    clogger::Logger::setTagColor("Connection", clogger::Color::BRIGHT_YELLOW);
    
    // Now all DatabaseLib logs show: [DatabaseLib][Tag]: message
    performQuery("SELECT * FROM users");  // Shows: [DatabaseLib][Query]: Executing: SELECT * FROM users
                                         // Shows: [DatabaseLib][Query]: Query completed successfully
    
    // Your own application logs don't have library prefix
    CLOG_INFO("App", "Application logic");  // Shows: [App]: Application logic
    
    return 0;
}
```

#### Multi-Library Scenario

```cpp
// Multiple libraries can coexist with different identifiers
void setupLibraries() {
    // Each library identifies itself
    networkLib::init();     // Calls setLibraryName("NetworkLib")
    databaseLib::init();    // Calls setLibraryName("DatabaseLib")
    audioLib::init();       // Calls setLibraryName("AudioLib")
    
    // Parent configures colors for each library
    clogger::Logger::enableLibraryTags(true);
    clogger::Logger::setLibraryColor("NetworkLib", clogger::Color::BRIGHT_MAGENTA);
    clogger::Logger::setLibraryColor("DatabaseLib", clogger::Color::BRIGHT_CYAN);
    clogger::Logger::setLibraryColor("AudioLib", clogger::Color::BRIGHT_YELLOW);
}

// Results in clear visual distinction:
// [NetworkLib][HTTP]: Server started
// [DatabaseLib][Query]: Connection established  
// [AudioLib][Device]: Audio device initialized
// [App]: Main application started
```

#### Benefits

- **Library Development**: Clean output during library development (tags disabled by default)
- **Integration Testing**: Easy identification of which library produced each log
- **Production Debugging**: Quickly isolate issues to specific libraries
- **No Code Changes**: Existing logging calls remain unchanged
- **Autonomous Libraries**: Libraries manage their own identification without parent coordination

#### Limitations

- Maximum library name length: 32 characters (configurable)
- Maximum of 16 different library colors (configurable)
- Library names are global state (last `setLibraryName()` call wins)

## Examples

The library includes comprehensive examples:

- **Desktop Example**: Basic usage with colored console output, tag color configuration, library tagging, and tag filtering demonstration
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

### Compile-time Optimization

CLog's primary performance advantage comes from compile-time filtering:

```cpp
#define CLOG_LEVEL 2  // Only ERROR and WARN levels
#include <clog/log.hpp>

// These are completely eliminated from the binary:
CLOG_INFO("App", "This code doesn't exist in the binary");
CLOG_DEBUG("App", "Neither does this");
CLOG_TRACE("App", "Or this");

// Only these generate code:
CLOG_ERROR("App", "Error message");  // ✓ Compiled
CLOG_WARN("App", "Warning message"); // ✓ Compiled
```

### Embedded Systems

- **Default buffer size**: 256 bytes (configurable via `CLOG_BUFFER_SIZE`)
- **Fixed-size buffers**: No dynamic allocation, stack-based formatting
- **Compile-time filtering**: Unused log levels generate zero code
- **Platform detection**: CMake-based configuration avoids runtime detection overhead
- **Minimal footprint**: Header-only design with embedded-specific optimizations

### Desktop Systems

- **Default buffer size**: 1024 bytes (larger for desktop workloads)
- **Color support**: ANSI escape codes for level and tag coloring
- **std::cout output**: Uses C++ streams instead of printf for desktop platforms
- **Full feature set**: All tag filtering, library identification, and color features enabled

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

