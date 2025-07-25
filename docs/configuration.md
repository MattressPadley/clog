# Configuration Guide

CLog is designed for compile-time configuration to achieve optimal performance. This guide covers all configuration options and patterns. For multi-library projects, see [Multi-Library Projects](multi-library-projects.md) for hierarchy best practices.

## Configuration Methods

### CMake Configuration (Recommended)

The preferred approach uses CMake's `target_compile_definitions` for type-safe, build-system integrated configuration.

#### For Libraries (use PRIVATE definitions)

```cmake
# Library configuration - sets reasonable defaults
add_library(my_awesome_lib src/network.cpp)
target_compile_definitions(my_awesome_lib PRIVATE 
    # Core configuration
    CLOG_LIBRARY_NAME="MyAwesomeLib"   # Library identification
    CLOG_LEVEL=3                       # INFO level - reasonable default
    CLOG_BUFFER_SIZE=512              # Message buffer size
    
    # Feature configuration  
    CLOG_ENABLE_TAG_FILTERING=1       # Enable tag filtering (default: enabled)
    CLOG_MAX_TAG_FILTERS=16           # Maximum number of tag filters
    CLOG_MAX_LIBRARY_COLORS=16        # Maximum number of library colors
    CLOG_MAX_LIBRARY_NAME_LENGTH=32   # Maximum library name length
)
target_link_libraries(my_awesome_lib PRIVATE clog::clog)
```

#### For Applications (override library settings)

```cmake
# Application configuration - controls all libraries
set(CLOG_LEVEL "4" CACHE STRING "Global log level for all components")

add_executable(my_app main.cpp)
target_compile_definitions(my_app PRIVATE 
    # Core configuration - overrides all library defaults
    CLOG_LEVEL=${CLOG_LEVEL}           # Application controls log level
    CLOG_BUFFER_SIZE=1024             # Larger buffer for applications
    
    # Platform configuration (optional - defaults to desktop) 
    CLOG_PLATFORM_DESKTOP=1           # Explicit desktop platform
    CLOG_HAS_COLOR_SUPPORT=1          # Enable colored output
    CLOG_HAS_PRINTF_SUPPORT=0         # Use std::cout (not printf)
    
    # Feature configuration
    CLOG_ENABLE_ASSERTS=1             # Enable assertion macros
)

# Include libraries - they inherit the application's CLOG_LEVEL
add_subdirectory(libs/MyAwesomeLib)
target_link_libraries(my_app PRIVATE my_awesome_lib clog::clog)
```

### Preprocessor Configuration (Alternative)

For projects not using CMake or when manual control is needed:

```cpp
// Define before including clog/log.hpp
#define CLOG_LEVEL 4                    // DEBUG level
#define CLOG_BUFFER_SIZE 512            // Message buffer size
#define CLOG_MAX_TAG_FILTERS 16         // Maximum number of tag filters
#define CLOG_ENABLE_TAG_FILTERING 1     // Enable tag filtering
#define CLOG_LIBRARY_NAME "MyLib"       // Library identification
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
    -DCLOG_LIBRARY_NAME=\"MyESP32Lib\"
```

## Configuration Options Reference

### Core Settings

#### `CLOG_LEVEL` (0-5)
Maximum log level - messages above this are compiled out entirely.

- `0` - `OFF` - No logging
- `1` - `ERROR` - Error messages only
- `2` - `WARN` - Warnings and errors
- `3` - `INFO` - Informational messages and above (default)
- `4` - `DEBUG` - Debug messages and above
- `5` - `TRACE` - All messages including trace

```cmake
target_compile_definitions(my_target PRIVATE CLOG_LEVEL=4)
```

#### `CLOG_BUFFER_SIZE` (64-4096)
Buffer size for formatted messages in bytes.

- **Default**: 256 bytes (embedded), 512 bytes (desktop)
- **Minimum**: 64 bytes
- **Maximum**: 4096 bytes (recommended limit)

```cmake
target_compile_definitions(my_target PRIVATE CLOG_BUFFER_SIZE=1024)
```

#### `CLOG_ENABLE_ASSERTS` (0/1)
Enable/disable assertion macros.

- **Default**: 1 (enabled) in debug builds, 0 (disabled) in release
- **Usage**: Enables `CLOG_ASSERT(condition, tag, message)` macro

```cmake
target_compile_definitions(my_target PRIVATE CLOG_ENABLE_ASSERTS=1)
```

### Platform Settings

#### `CLOG_PLATFORM_*` 
Explicit platform selection to override auto-detection.

Available platforms:
- `CLOG_PLATFORM_ARDUINO=1`
- `CLOG_PLATFORM_ESP32=1`
- `CLOG_PLATFORM_ESP8266=1`
- `CLOG_PLATFORM_RP2040_ARDUINO=1`
- `CLOG_PLATFORM_RP2040_SDK=1`
- `CLOG_PLATFORM_ESP_IDF=1`
- `CLOG_PLATFORM_DESKTOP=1`
- `CLOG_PLATFORM_WINDOWS=1`
- `CLOG_PLATFORM_LINUX=1`
- `CLOG_PLATFORM_MACOS=1`

```cmake
target_compile_definitions(my_target PRIVATE CLOG_PLATFORM_ESP32=1)
```

#### `CLOG_HAS_COLOR_SUPPORT` (0/1)
Enable colored console output.

- **Default**: 1 (enabled) on desktop, 0 (disabled) on embedded
- **Effect**: Enables ANSI escape codes for colored output

```cmake
target_compile_definitions(my_target PRIVATE CLOG_HAS_COLOR_SUPPORT=1)
```

#### `CLOG_HAS_PRINTF_SUPPORT` (0/1)
Choose output method: printf vs std::cout.

- **Default**: 0 (std::cout) on desktop, 1 (printf) on embedded
- **Effect**: 0 = uses std::cout, 1 = uses printf/Serial.print

```cmake
target_compile_definitions(my_target PRIVATE CLOG_HAS_PRINTF_SUPPORT=0)
```

### Feature Settings

#### `CLOG_ENABLE_TAG_FILTERING` (0/1)
Enable tag-based filtering system.

- **Default**: 1 (enabled)
- **Effect**: Enables tag whitelist/blacklist filtering

```cmake
target_compile_definitions(my_target PRIVATE CLOG_ENABLE_TAG_FILTERING=1)
```

#### `CLOG_MAX_TAG_FILTERS` (1-64)
Maximum number of simultaneous tag filters.

- **Default**: 16
- **Effect**: Sets size of internal tag filter array

```cmake
target_compile_definitions(my_target PRIVATE CLOG_MAX_TAG_FILTERS=32)
```

#### `CLOG_MAX_TAG_LENGTH` (4-64)
Maximum length for tag names.

- **Default**: 16 characters
- **Effect**: Tags longer than this are truncated

```cmake
target_compile_definitions(my_target PRIVATE CLOG_MAX_TAG_LENGTH=24)
```

### Library Identification Settings

#### `CLOG_LIBRARY_NAME` (string)
Compile-time library name for nested dependency identification.

- **Default**: Not set
- **Effect**: Embeds library name at compile time for parent visibility control

```cmake
target_compile_definitions(my_lib PRIVATE CLOG_LIBRARY_NAME=\"DatabaseLib\")
```

#### `CLOG_MAX_LIBRARY_COLORS` (1-32)
Maximum number of library color configurations.

- **Default**: 16
- **Effect**: Limits library-specific color assignments

```cmake
target_compile_definitions(my_target PRIVATE CLOG_MAX_LIBRARY_COLORS=32)
```

#### `CLOG_MAX_LIBRARY_NAME_LENGTH` (4-64)
Maximum length for library names.

- **Default**: 32 characters
- **Effect**: Library names longer than this are truncated

```cmake
target_compile_definitions(my_target PRIVATE CLOG_MAX_LIBRARY_NAME_LENGTH=48)
```

## Platform-Specific Configuration Examples

### Desktop Development

```cmake
add_executable(desktop_app main.cpp)
target_compile_definitions(desktop_app PRIVATE
    CLOG_LEVEL=5                    # TRACE level for development
    CLOG_BUFFER_SIZE=1024          # Large buffer
    CLOG_HAS_COLOR_SUPPORT=1       # Enable colors
    CLOG_HAS_PRINTF_SUPPORT=0      # Use std::cout
    CLOG_ENABLE_ASSERTS=1          # Enable assertions
)
target_link_libraries(desktop_app PRIVATE clog::clog)
```

### ESP32 Production

```cmake
add_executable(esp32_app main.cpp)
target_compile_definitions(esp32_app PRIVATE
    CLOG_LEVEL=2                   # WARN level for production
    CLOG_BUFFER_SIZE=256           # Smaller buffer
    CLOG_PLATFORM_ESP32=1          # Explicit platform
    CLOG_HAS_COLOR_SUPPORT=0       # No colors for embedded
    CLOG_HAS_PRINTF_SUPPORT=1      # Use Serial.print
    CLOG_ENABLE_ASSERTS=0          # Disable assertions
)
target_link_libraries(esp32_app PRIVATE clog::clog)
```

### Arduino Development

```cpp
// In Arduino sketch
#define CLOG_LEVEL 4               // DEBUG level
#define CLOG_BUFFER_SIZE 256       // Embedded buffer size
#define CLOG_PLATFORM_ARDUINO      // Explicit platform
#include <clog/log.hpp>

void setup() {
    Serial.begin(115200);
    CLOG_INFO("Arduino", "Sketch started");
}
```

### RP2040 with Pico SDK

```cmake
add_executable(pico_app main.cpp)
target_compile_definitions(pico_app PRIVATE
    CLOG_LEVEL=3                   # INFO level
    CLOG_BUFFER_SIZE=512           # Medium buffer
    CLOG_PLATFORM_RP2040_SDK=1     # Explicit platform
    CLOG_HAS_PRINTF_SUPPORT=1      # Use printf
)
target_link_libraries(pico_app PRIVATE clog::clog pico_stdlib)
```

## Configuration Validation

CLog validates configuration at compile time through static assertions:

```cpp
// From config.hpp - these checks happen automatically
static_assert(CLOG_LEVEL >= 0 && CLOG_LEVEL <= 5, "CLOG_LEVEL must be between 0 and 5");
static_assert(CLOG_BUFFER_SIZE >= 64, "CLOG_BUFFER_SIZE must be at least 64 bytes");
static_assert(CLOG_BUFFER_SIZE <= 4096, "CLOG_BUFFER_SIZE should not exceed 4096 bytes");
```

If you see compilation errors about these assertions, check your configuration values.

## Runtime Configuration Query

You can query the effective configuration at runtime:

```cpp
#include <clog/log.hpp>
#include <clog/config.hpp>

int main() {
    // Print compile-time configuration
    printf("Effective CLOG_LEVEL: %d\n", CLOG_LEVEL);
    printf("Buffer size: %zu bytes\n", clogger::config::BUFFER_SIZE);
    printf("Max tag filters: %zu\n", clogger::config::MAX_TAG_FILTERS);
    
    // Complete configuration summary
    clogger::config::printConfig();  // Desktop only
    
    return 0;
}
```

## Troubleshooting

### Common Configuration Issues

**Issue: Log level not taking effect**
- **Check**: Verify `CLOG_LEVEL` is defined before including headers
- **Fix**: Use CMake `target_compile_definitions` or `#define CLOG_LEVEL N` before `#include`

**Issue: Buffer overflow warnings**
- **Check**: Messages longer than `CLOG_BUFFER_SIZE`
- **Fix**: Increase buffer size or shorten messages

**Issue: Colors not working**
- **Check**: `CLOG_HAS_COLOR_SUPPORT=1` and platform supports ANSI codes
- **Fix**: Enable color support or check terminal compatibility

**Issue: Library name not appearing**
- **Check**: `CLOG_LIBRARY_NAME` defined and library tags enabled
- **Fix**: Set library name and call `clogger::Logger::enableLibraryTags(true)`

**Issue: Tag filtering not working**
- **Check**: `CLOG_ENABLE_TAG_FILTERING=1` and tags properly configured
- **Fix**: Enable tag filtering and verify tag configuration calls

### Configuration Verification

Test your configuration with this verification code:

```cpp
#include <clog/log.hpp>

int main() {
    // This should show your configured level
    printf("Configured max level: %d\n", CLOG_LEVEL);
    
    // Test all levels
    CLOG_ERROR("Config", "ERROR level test");
    CLOG_WARN("Config", "WARN level test");  
    CLOG_INFO("Config", "INFO level test");
    CLOG_DEBUG("Config", "DEBUG level test");
    CLOG_TRACE("Config", "TRACE level test");
    
    return 0;
}
```

Only messages at or below your configured `CLOG_LEVEL` should appear.

## Next Steps

- **[Multi-Library Projects](multi-library-projects.md)** - Log level hierarchy best practices
- **[Platform Support](platform-support.md)** - Platform-specific configuration guides
- **[API Reference](api-reference.md)** - Complete API documentation
- **[Performance](performance.md)** - Optimization and performance considerations