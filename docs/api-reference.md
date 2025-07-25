# API Reference

Complete reference for all CLog APIs, macros, and configuration options.

## Log Levels

CLog defines five log levels with compile-time and runtime filtering support:

```cpp
enum class Level {
    OFF = 0,    // No logging
    ERROR = 1,  // Error messages only
    WARN = 2,   // Warnings and errors
    INFO = 3,   // Informational messages and above (default)
    DEBUG = 4,  // Debug messages and above
    TRACE = 5   // All messages including trace
};
```

- `clogger::Level::OFF` - No logging
- `clogger::Level::ERROR` - Error messages only
- `clogger::Level::WARN` - Warnings and errors
- `clogger::Level::INFO` - Informational messages and above
- `clogger::Level::DEBUG` - Debug messages and above
- `clogger::Level::TRACE` - All messages including trace

## Logging Macros

### Primary Logging Macros

```cpp
CLOG_ERROR(tag, format, ...)   // Error level (1)
CLOG_WARN(tag, format, ...)    // Warning level (2)
CLOG_INFO(tag, format, ...)    // Info level (3)
CLOG_DEBUG(tag, format, ...)   // Debug level (4)
CLOG_TRACE(tag, format, ...)   // Trace level (5)
```

**Parameters:**
- `tag` - String identifier for the log source (e.g., "Network", "Database")
- `format` - printf-style format string
- `...` - Variable arguments for format string

**Example:**
```cpp
CLOG_INFO("Network", "Connected to server %s:%d", hostname, port);
CLOG_ERROR("Database", "Connection failed with error code: %d", errorCode);
```

### Assertion Macro

```cpp
CLOG_ASSERT(condition, tag, format, ...)
```

Logs an error message if condition is false. Only available when `CLOG_ENABLE_ASSERTS=1`.

**Example:**
```cpp
CLOG_ASSERT(ptr != nullptr, "Memory", "Null pointer detected");
CLOG_ASSERT(size > 0, "Buffer", "Invalid buffer size: %d", size);
```

## Logger Class

The `clogger::Logger` class provides the main API for configuration and control.

### Log Level Management

```cpp
// Runtime level configuration (use sparingly - compile-time preferred)
static void setLevel(Level level);
static Level getLevel();

// Check if a level is enabled
static bool isLevelEnabled(Level level);
```

**Example:**
```cpp
// Runtime configuration (less performant than compile-time)
clogger::Logger::setLevel(clogger::Level::DEBUG);
Level current = clogger::Logger::getLevel();

// Check before expensive operations
if (clogger::Logger::isLevelEnabled(clogger::Level::DEBUG)) {
    std::string expensiveDebugInfo = generateDebugReport();
    CLOG_DEBUG("Debug", "Report: %s", expensiveDebugInfo.c_str());
}
```

### Callback Integration

```cpp
// Callback function signature
typedef void (*LogCallback)(Level level, const char* tag, const char* message, const char* libraryName);

// Callback management
static void setCallback(LogCallback callback);
static void clearCallback();
static bool hasCallback();

// Direct output control
static void enableDirectOutput(bool enabled);
static bool isDirectOutputEnabled();
```

**Example:**
```cpp
void myLogHandler(clogger::Level level, const char* tag, const char* message, const char* libraryName) {
    // Route to existing logging system
    printf("[%s] %s: %s\n", levelToString(level), tag, message);
    
    // Handle library identification
    if (libraryName && strlen(libraryName) > 0) {
        printf("  (from library: %s)\n", libraryName);
    }
}

// Install callback
clogger::Logger::setCallback(myLogHandler);

// Optionally disable direct output to avoid double-logging
clogger::Logger::enableDirectOutput(false);
```

### Platform Configuration

```cpp
// Platform enumeration
enum class Platform {
    ARDUINO, ESP32, ESP8266, RP2040_ARDUINO, RP2040_SDK,
    ESP_IDF, DESKTOP, WINDOWS, LINUX, MACOS, AUTO_DETECT
};

// Platform management
static void init();                        // Auto-detect platform
static void init(Platform platform);      // Explicit platform
static void setPlatform(Platform platform);
static Platform getPlatform();

// Platform queries
static bool isDesktopPlatform();
static bool isEmbeddedPlatform();
static bool hasColorSupport();
static bool hasPrintfSupport();
```

**Example:**
```cpp
// Explicit platform configuration
clogger::Logger::init(clogger::Platform::RP2040_SDK);

// Query platform capabilities
if (clogger::Logger::hasColorSupport()) {
    clogger::Logger::setTagColor("Network", clogger::Color::BRIGHT_CYAN);
}

// Platform-specific logic
if (clogger::Logger::isEmbeddedPlatform()) {
    // Use smaller buffers, disable features for embedded
}
```

## Tag Filtering

### Tag Management

```cpp
// Enable/disable specific tags
static void enableTag(const char* tag);
static void disableTag(const char* tag);

// Bulk operations
static void enableAllTags();    // Allow all tags (default mode)
static void disableAllTags();   // Block all tags (empty whitelist)

// Query tag status
static bool isTagEnabled(const char* tag);

// Clear all filters
static void clearTagFilters();
```

### Tag Filtering Modes

- **ALLOW_ALL** (default): No tag filtering, all tags are logged
- **WHITELIST**: Only explicitly enabled tags are logged (set by calling `enableTag()`)
- **BLACKLIST**: All tags are logged except explicitly disabled ones (set by calling `disableTag()`)

### Tag Filtering Example

```cpp
#define CLOG_LEVEL 4  // DEBUG level
#include <clog/log.hpp>

int main() {
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

### Filtering Integration

Tag filtering and log level filtering work together:

1. **Compile-time filtering (CLOG_LEVEL)**: Messages above the compile-time level are completely eliminated from the binary
2. **Runtime level filtering**: For compiled messages, runtime level is checked
3. **Tag filtering**: Then tag filters are applied
4. **Result**: Only messages passing all checks are logged

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

## Color Configuration

### Available Colors

```cpp
enum class Color {
    DEFAULT,        // No color (system default)
    BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE,
    BRIGHT_BLACK, BRIGHT_RED, BRIGHT_GREEN, BRIGHT_YELLOW,
    BRIGHT_BLUE, BRIGHT_MAGENTA, BRIGHT_CYAN, BRIGHT_WHITE
};
```

### Tag Color Management

```cpp
// Set custom colors for specific tags
static void setTagColor(const char* tag, Color color);
static void clearTagColor(const char* tag);
static void clearAllTagColors();
```

### Tag Color Example

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

### Output Format

The colored output format is: `[<colored_level>] <colored_tag>: message`

- **Level text** is colored based on log level (ERROR=red, WARN=yellow, INFO=green, etc.)
- **Tag text** is colored based on your configuration (or default if not configured)
- **Everything else** (brackets, colon, message) remains in default color

### Color Platform Support

- **Desktop platforms**: Full color support with ANSI escape codes
- **Embedded platforms**: Tag colors are ignored to save memory; only level colors are used
- **Arduino/ESP32**: Colors are not displayed but tag color calls are safely ignored

### Color Limitations

- Maximum of 32 different tag colors can be configured simultaneously
- Tag names are limited to 31 characters
- Colors only affect direct console output (not callback-based logging)

## Library Tagging

### Library Identification Methods

```cpp
// Compile-time library identification (recommended)
// Set via CMake: target_compile_definitions(my_lib PRIVATE CLOG_LIBRARY_NAME="MyLib")

// Runtime library identification (alternative)
static void setLibraryName(const char* name);
static const char* getLibraryName();
```

### Library Tagging Control

```cpp
// Visibility control (called by parent applications)
static void enableLibraryTags(bool enabled);
static bool isLibraryTagsEnabled();

// Library-specific colors (independent from tag colors)
static void setLibraryColor(const char* libraryName, Color color);
static void clearLibraryColor(const char* libraryName);
static void clearAllLibraryColors();
```

### Library Tagging Example

**Compile-time Approach (Recommended):**

```cmake
# In MyDatabaseLib/CMakeLists.txt
add_library(database_lib database.cpp)
target_compile_definitions(database_lib PRIVATE CLOG_LIBRARY_NAME="DatabaseLib")
target_link_libraries(database_lib PRIVATE clog::clog)
```

```cpp
// In MyDatabaseLib/src/database.cpp
#include <clog/log.hpp>  // Library name already configured via CMake

void initDatabase() {
    // No initialization call needed - library name is embedded at compile time
    CLOG_INFO("Init", "Database library initialized");  // [DatabaseLib][Init]: message
}
```

**Runtime Approach (Alternative):**

```cpp
// In your library code
#include <clog/log.hpp>

void initDatabase() {
    // Library identifies itself during initialization
    clogger::Logger::setLibraryName("DatabaseLib");
    
    // Normal logging - no changes needed to existing code
    CLOG_INFO("Init", "Database library initialized");  // Shows: [Init]: message
    CLOG_INFO("Connection", "Connected to server");      // Shows: [Connection]: message
}
```

**Parent Application Control:**

```cpp
// In parent application
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
    
    // Now all DatabaseLib logs show: [DatabaseLib][Tag]: message
    performQuery("SELECT * FROM users");
    
    // Your own application logs don't have library prefix
    CLOG_INFO("App", "Application logic");  // Shows: [App]: Application logic
    
    return 0;
}
```

### Library Tagging Output Format

- **Library tags disabled** (default): `[ERROR] [Tag]: message`
- **Library tags enabled**: `[ERROR] [LibraryName][Tag]: message`
- **With colors**: `[ERROR] [cyan_LibraryName][green_Tag]: message`

### Library Tagging Benefits

- **Library Development**: Clean output during library development (tags disabled by default)
- **Integration Testing**: Easy identification of which library produced each log
- **Production Debugging**: Quickly isolate issues to specific libraries
- **No Code Changes**: Existing logging calls remain unchanged
- **Autonomous Libraries**: Libraries manage their own identification without parent coordination

### Library Tagging Limitations

- Maximum library name length: 32 characters (configurable via `CLOG_MAX_LIBRARY_NAME_LENGTH`)
- Maximum of 16 different library colors (configurable via `CLOG_MAX_LIBRARY_COLORS`)
- Library names are global state (last `setLibraryName()` call wins in runtime approach)

## Configuration Query

### Runtime Configuration Query

```cpp
#include <clog/config.hpp>

// Query effective compile-time configuration
printf("Effective CLOG_LEVEL: %d\n", CLOG_LEVEL);
printf("Buffer size: %zu bytes\n", clogger::config::BUFFER_SIZE);
printf("Max tag filters: %zu\n", clogger::config::MAX_TAG_FILTERS);

// Complete configuration summary (desktop only)
clogger::config::printConfig();
```

### Configuration Constants

All compile-time configuration is available as constants in the `clogger::config` namespace:

```cpp
namespace clogger::config {
    constexpr int DEFAULT_LEVEL = CLOG_LEVEL;
    constexpr size_t BUFFER_SIZE = CLOG_BUFFER_SIZE;
    constexpr size_t MAX_TAG_LENGTH = CLOG_MAX_TAG_LENGTH;
    constexpr size_t MAX_TAG_FILTERS = CLOG_MAX_TAG_FILTERS;
    constexpr bool TAG_FILTERING_ENABLED = CLOG_ENABLE_TAG_FILTERING;
    constexpr size_t MAX_LIBRARY_COLORS = CLOG_MAX_LIBRARY_COLORS;
    constexpr size_t MAX_LIBRARY_NAME_LENGTH = CLOG_MAX_LIBRARY_NAME_LENGTH;
    constexpr bool COLORS_ENABLED = CLOG_ENABLE_COLORS;
    constexpr bool ASSERTS_ENABLED = CLOG_ENABLE_ASSERTS;
}
```

## Thread Safety

CLog is designed to be thread-safe on platforms that support it:

- **Desktop platforms**: Thread-safe logging with mutex protection
- **ESP32/ESP-IDF**: Thread-safe with FreeRTOS mutex
- **Arduino/single-threaded**: No threading overhead
- **RP2040**: Core-safe operation with Pico SDK

## Error Handling

CLog is designed to be robust and never crash your application:

- **Buffer overflow**: Messages are truncated to fit configured buffer size
- **Invalid parameters**: Safely ignored with no side effects
- **Memory allocation**: Uses only stack-based fixed buffers
- **Platform detection**: Falls back to safe defaults if detection fails

## Performance Characteristics

### Compile-time Optimization

- **Eliminated calls**: Messages above `CLOG_LEVEL` generate zero code
- **No function calls**: Disabled levels compile to no-ops
- **Zero runtime cost**: Unused log levels have no performance impact

### Runtime Performance

- **Fixed buffers**: No dynamic allocation, stack-based formatting
- **Minimal overhead**: Optimized for embedded systems
- **Tag filtering**: O(1) lookup for enabled/disabled tags
- **Color support**: Minimal impact when enabled

## Next Steps

- **[Configuration Guide](configuration.md)** - Comprehensive configuration options
- **[Multi-Library Projects](multi-library-projects.md)** - Best practices for complex projects
- **[Platform Support](platform-support.md)** - Platform-specific features and limitations
- **[Performance Guide](performance.md)** - Optimization strategies and benchmarks