# Multi-Library Projects Guide

When using CLog across multiple libraries in a single project, it's essential to establish proper log level hierarchy where the top-level application controls log levels for all dependencies. This ensures consistent behavior and allows application developers to control verbosity across their entire codebase.

## Understanding the Hierarchy

CLog's hierarchy system works through standard C preprocessor and CMake patterns:

1. **Libraries set defaults** using `PRIVATE` compile definitions for standalone development
2. **Applications override** library settings using their own compile definitions  
3. **Top-level wins** through CMake's target-specific definition precedence

## For Library Authors

### ✅ Recommended Pattern

```cmake
# In your library's CMakeLists.txt
add_library(my_awesome_lib src/network.cpp src/database.cpp)
target_compile_definitions(my_awesome_lib PRIVATE
    CLOG_LIBRARY_NAME="MyAwesomeLib"
    CLOG_LEVEL=3  # INFO level - reasonable default for development
)
target_link_libraries(my_awesome_lib PRIVATE clog::clog)
```

### ❌ Avoid These Patterns

```cmake
# DON'T: Forces your log level on all consumers
target_compile_definitions(my_awesome_lib PUBLIC CLOG_LEVEL=4)
target_compile_definitions(my_awesome_lib INTERFACE CLOG_LEVEL=4)

# DON'T: Global definitions leak to consumers  
add_compile_definitions(CLOG_LEVEL=4)
```

### Code-Level Best Practice

```cpp
// In your library source files
#ifndef CLOG_LEVEL
    #define CLOG_LEVEL 3  // Set reasonable default if not configured
#endif
#include <clog/log.hpp>

namespace MyAwesomeLib {
    void initNetwork() {
        CLOG_INFO("Network", "Initializing network subsystem");
        // Your library code continues to work identically
        // whether used standalone or as a dependency
    }
}
```

## For Application Developers

### Method 1: Global Cache Variable (Recommended)

```cmake
# Set global log level that overrides all library defaults
set(CLOG_LEVEL "4" CACHE STRING "Global log level for all components")

add_executable(my_app main.cpp)
target_compile_definitions(my_app PRIVATE CLOG_LEVEL=${CLOG_LEVEL})

# Include your libraries normally
add_subdirectory(libs/MyAwesomeLib)
add_subdirectory(libs/DatabaseLib)
target_link_libraries(my_app PRIVATE my_awesome_lib database_lib clog::clog)

# Users can override: cmake -DCLOG_LEVEL=2 ..
```

### Method 2: Global Override for All Targets

```cmake
# Apply to all targets in this CMake project
add_compile_definitions(CLOG_LEVEL=4)

add_executable(my_app main.cpp)
add_subdirectory(libs/MyAwesomeLib)  # Inherits the global definition
target_link_libraries(my_app PRIVATE my_awesome_lib clog::clog)
```

### Method 3: Per-Target Control

```cmake
add_executable(my_app main.cpp)
target_compile_definitions(my_app PRIVATE
    CLOG_LEVEL=5  # TRACE level for this specific target
)

add_library(my_service service.cpp)
target_compile_definitions(my_service PRIVATE
    CLOG_LEVEL=2  # WARN level for this service
)

target_link_libraries(my_app PRIVATE my_service clog::clog)
```

## Complete Multi-Library Example

### Library A (NetworkLib/CMakeLists.txt)

```cmake
add_library(network_lib src/network.cpp)
target_compile_definitions(network_lib PRIVATE
    CLOG_LIBRARY_NAME="NetworkLib"
    CLOG_LEVEL=3  # INFO default for network library development
)
target_link_libraries(network_lib PRIVATE clog::clog)
```

### Library B (DatabaseLib/CMakeLists.txt)

```cmake
add_library(database_lib src/database.cpp)
target_compile_definitions(database_lib PRIVATE
    CLOG_LIBRARY_NAME="DatabaseLib"  
    CLOG_LEVEL=3  # INFO default for database library development
)
target_link_libraries(database_lib PRIVATE clog::clog)
```

### Application (MyApp/CMakeLists.txt)

```cmake
# Application has final control over all log levels
set(CLOG_LEVEL "5" CACHE STRING "Global application log level")

add_executable(my_app main.cpp)
target_compile_definitions(my_app PRIVATE CLOG_LEVEL=${CLOG_LEVEL})

# Include libraries with their defaults
add_subdirectory(libs/NetworkLib)
add_subdirectory(libs/DatabaseLib)
target_link_libraries(my_app PRIVATE network_lib database_lib clog::clog)
```

### Result

- **Library development**: Each library uses its INFO level (3) default
- **Application build**: All libraries use TRACE level (5) set by application
- **User override**: `cmake -DCLOG_LEVEL=1 ..` sets ERROR level for everything

## Verification & Testing

### Runtime Verification

```cpp
#include <clog/log.hpp>
#include <clog/config.hpp>

int main() {
    // Display effective configuration
    printf("=== CLog Configuration ===\n");
    printf("Compile-time CLOG_LEVEL: %d\n", CLOG_LEVEL);
    clogger::config::printConfig();
    
    // Test hierarchy is working
    printf("\n=== Testing Log Levels ===\n");
    CLOG_ERROR("Test", "ERROR (1) - should show if level >= 1");
    CLOG_WARN("Test", "WARN (2) - should show if level >= 2");  
    CLOG_INFO("Test", "INFO (3) - should show if level >= 3");
    CLOG_DEBUG("Test", "DEBUG (4) - should show if level >= 4");
    CLOG_TRACE("Test", "TRACE (5) - should show if level >= 5");
    
    return 0;
}
```

### Build-time Verification

```bash
# Check what CLOG_LEVEL is actually being compiled with
cmake --build . --verbose 2>&1 | grep CLOG_LEVEL

# Or examine compile commands directly
grep -o "CLOG_LEVEL=[0-9]" build/compile_commands.json | sort | uniq
```

## Troubleshooting Common Issues

### Issue: "My library's log level isn't being overridden"
- **Check**: Library uses `PRIVATE` definitions, not `PUBLIC`/`INTERFACE`
- **Fix**: Change to `target_compile_definitions(my_lib PRIVATE CLOG_LEVEL=3)`

### Issue: "Application level not applying to libraries"
- **Check**: Application target defines `CLOG_LEVEL` for itself
- **Fix**: Ensure `target_compile_definitions(my_app PRIVATE CLOG_LEVEL=4)`

### Issue: "Different libraries showing different log levels"  
- **Check**: Each library has different `PRIVATE` defaults
- **Fix**: Use global `add_compile_definitions(CLOG_LEVEL=4)` in application

### Issue: "Log level seems to be ignored"
- **Check**: Code uses runtime `setLevel()` calls that override compile-time setting
- **Fix**: Remove runtime calls, rely on compile-time `CLOG_LEVEL` for performance

### Issue: "Can't tell which library a log message came from"
- **Check**: Library tagging not enabled
- **Fix**: Use `clogger::Logger::enableLibraryTags(true)` in application

## Integration Best Practices

1. **Document your library's default level** in README/documentation
2. **Use INFO (3) as default** for most libraries - good balance of usefulness vs noise  
3. **Test both standalone and integrated builds** to ensure hierarchy works
4. **Provide CMake cache variables** in applications for easy user control
5. **Use library tagging** to distinguish logs from different components

## Library Tagging for Multi-Library Projects

When working with multiple libraries, library tagging helps identify which library produced each log message.

### Compile-Time Library Identification (Recommended)

The preferred approach uses CMake to embed library names at compile time:

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

### Multi-Library Integration Example

```cmake
# In parent application CMakeLists.txt
add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE database_lib network_lib clog::clog)
```

```cpp
// In parent application main.cpp
int main() {
    // Enable library identification  
    clogger::Logger::enableLibraryTags(true);
    clogger::Logger::setLibraryColor("DatabaseLib", clogger::Color::CLOG_BRIGHT_BLUE);
    clogger::Logger::setLibraryColor("NetworkLib", clogger::Color::CLOG_BRIGHT_GREEN);
    
    initDatabase();    // [DatabaseLib][Init]: Database library initialized
    initNetwork();     // [NetworkLib][Init]: Network library initialized
    
    return 0;
}
```

### Output Format

- **Library tags disabled** (default): `[ERROR] [Tag]: message`
- **Library tags enabled**: `[ERROR] [LibraryName][Tag]: message`
- **With colors**: `[ERROR] [cyan_Library][green_Tag]: message`

## Advanced: Per-Library Level Control

For applications needing fine-grained control over individual library log levels:

```cmake
# Define log levels for each library
set(NETWORK_LOG_LEVEL "4" CACHE STRING "NetworkLib log level")  
set(DATABASE_LOG_LEVEL "2" CACHE STRING "DatabaseLib log level")
set(APP_LOG_LEVEL "3" CACHE STRING "Application log level")

# Apply to specific targets
target_compile_definitions(network_lib PRIVATE CLOG_LEVEL=${NETWORK_LOG_LEVEL})
target_compile_definitions(database_lib PRIVATE CLOG_LEVEL=${DATABASE_LOG_LEVEL})
target_compile_definitions(my_app PRIVATE CLOG_LEVEL=${APP_LOG_LEVEL})

# Users can fine-tune: cmake -DNETWORK_LOG_LEVEL=5 -DDATABASE_LOG_LEVEL=1 ..
```

This approach provides maximum flexibility while maintaining clear hierarchy and control.

## Real-World Scenario: Slide Kick Project

The Slide Kick project demonstrates multi-library CLog integration:

```
SlideKick/
├── sk-slide/          # Motorized fader library (uses CLog)
├── sk-oal/           # OSC abstraction layer (uses CLog)  
├── firmware/esp32/   # ESP32 main controller (uses both libraries)
└── tools/desktop/    # Desktop tools (uses OSC library)
```

**Library configurations:**
```cmake
# sk-slide/CMakeLists.txt
target_compile_definitions(sk_slide PRIVATE 
    CLOG_LIBRARY_NAME="sk-slide"
    CLOG_LEVEL=3  # INFO default
)

# sk-oal/CMakeLists.txt  
target_compile_definitions(sk_oal PRIVATE
    CLOG_LIBRARY_NAME="sk-oal"
    CLOG_LEVEL=3  # INFO default
)
```

**Application control:**
```cmake
# firmware/esp32/CMakeLists.txt
set(CLOG_LEVEL "2" CACHE STRING "Production log level")  # WARN for production
target_compile_definitions(esp32_firmware PRIVATE CLOG_LEVEL=${CLOG_LEVEL})

# tools/desktop/CMakeLists.txt
set(CLOG_LEVEL "5" CACHE STRING "Desktop log level")  # TRACE for development
target_compile_definitions(desktop_tool PRIVATE CLOG_LEVEL=${CLOG_LEVEL})
```

This setup allows:
- Libraries to have reasonable INFO defaults for development
- ESP32 firmware to use WARN level for production (saves flash/RAM)
- Desktop tools to use TRACE level for full debugging
- Individual library levels can be overridden when needed

## Next Steps

- **[Configuration](configuration.md)** - Detailed configuration options
- **[API Reference](api-reference.md)** - Complete API documentation
- **[Performance](performance.md)** - Optimization for multi-library projects