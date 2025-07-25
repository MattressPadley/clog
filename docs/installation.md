# Installation Guide

CLog is a header-only library that can be integrated into your project through several methods. Choose the approach that best fits your build system and project structure.

## Git Submodule (Recommended)

The most common way to integrate CLog is as a Git submodule:

```bash
# Add CLog to your project
git submodule add https://github.com/MattressPadley/clog.git external/clog
git submodule update --init --recursive
```

## CMake Integration

### Basic Integration

```cmake
# Add CLog to your CMakeLists.txt
add_subdirectory(external/clog)
target_link_libraries(your_target clog::clog)
```

### Complete Example

```cmake
cmake_minimum_required(VERSION 3.12)
project(my_project)

# Add CLog
add_subdirectory(external/clog)

# Create your executable
add_executable(my_app main.cpp)
target_compile_definitions(my_app PRIVATE CLOG_LEVEL=4)
target_link_libraries(my_app PRIVATE clog::clog)
```

### Nested Dependencies (Advanced)

When using CLog as a submodule in multiple nested projects (e.g., a library that uses CLog is itself used by an application that also uses CLog), use this pattern to avoid duplicate target definitions:

```cmake
# Recommended pattern for nested submodule usage
find_package(clog QUIET)
if(NOT clog_FOUND)
    add_subdirectory(external/clog)
endif()

target_link_libraries(your_target clog::clog)
```

#### Example Scenario

Consider this project structure:
```
MyProject/
├── external/clog/           # clog submodule
├── MyLibrary/
│   ├── external/clog/       # same clog submodule
│   └── CMakeLists.txt       # uses find_package fallback pattern
└── CMakeLists.txt           # uses find_package fallback pattern
```

**What happens:**
1. **First include**: `find_package(clog QUIET)` fails, `add_subdirectory()` runs, creates `clog::clog` target
2. **Subsequent includes**: `find_package(clog QUIET)` succeeds (finds existing target), `add_subdirectory()` is skipped
3. **No duplicate targets**: CMake automatically handles the deduplication

**Benefits:**
- No duplicate target definition errors
- Automatic deduplication through CMake's export/import system
- Works with any level of nesting
- Compatible with existing CMake workflows

### Alternative: Target Guard Pattern

If you prefer explicit control, add a target guard:

```cmake
# At the top of any CMakeLists.txt that includes clog
if(TARGET clog::clog)
    return()
endif()
add_subdirectory(external/clog)
```

## PlatformIO Integration

For PlatformIO projects, add CLog to your `platformio.ini`:

```ini
; Add to platformio.ini
[env:your_env]
lib_deps = 
    file://external/clog
build_flags =
    -DCLOG_LEVEL=4  ; Debug level
    -DCLOG_PLATFORM=ESP32  ; Explicit platform
```

### Complete PlatformIO Example

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino

lib_deps = 
    file://external/clog

build_flags =
    -DCLOG_LEVEL=3          ; INFO level
    -DCLOG_BUFFER_SIZE=256  ; Smaller buffer for embedded
    -DCLOG_PLATFORM=ESP32   ; Explicit platform
```

## Arduino IDE Integration

For Arduino IDE projects:

1. **Copy the library**: Copy the entire `clog` folder to your Arduino libraries directory:
   - Windows: `Documents/Arduino/libraries/`
   - macOS: `~/Documents/Arduino/libraries/`
   - Linux: `~/Arduino/libraries/`

2. **Include in your sketch**:
   ```cpp
   #define CLOG_LEVEL 3  // Set before including
   #include <clog/log.hpp>
   
   void setup() {
       Serial.begin(115200);
       CLOG_INFO("Arduino", "Sketch started");
   }
   ```

## Direct File Copy

For simple projects or when you can't use Git submodules:

1. **Download the headers**: Copy these files to your project:
   - `include/clog/log.hpp`
   - `include/clog/platform.hpp`
   - `include/clog/config.hpp`

2. **Add to include path**: Ensure your compiler can find the headers:
   ```cpp
   #include "clog/log.hpp"  // Adjust path as needed
   ```

## Package Managers

### vcpkg (Future)

CLog support for vcpkg is planned but not yet available.

### Conan (Future)

CLog support for Conan is planned but not yet available.

## Verification

After installation, verify CLog works with this simple test:

### CMake Test

```cmake
# test_clog/CMakeLists.txt
cmake_minimum_required(VERSION 3.12)
project(test_clog)

add_subdirectory(../path/to/clog clog)

add_executable(test_app test.cpp)
target_compile_definitions(test_app PRIVATE CLOG_LEVEL=4)
target_link_libraries(test_app PRIVATE clog::clog)
```

```cpp
// test_clog/test.cpp
#include <clog/log.hpp>

int main() {
    CLOG_INFO("Test", "CLog is working correctly!");
    CLOG_DEBUG("Test", "Debug message with parameter: %d", 42);
    return 0;
}
```

### Build and Run

```bash
cd test_clog
mkdir build && cd build
cmake ..
make
./test_app
```

Expected output:
```
[INFO] Test: CLog is working correctly!
[DEBUG] Test: Debug message with parameter: 42
```

## Troubleshooting

### Common Issues

**"Target clog::clog already exists"**
- Use the [nested dependencies pattern](#nested-dependencies-advanced) above
- Or add target guard pattern to prevent duplicate includes

**"clog/log.hpp not found"**
- Verify the submodule was added correctly: `git submodule status`
- Check CMake includes the correct path: `add_subdirectory(external/clog)`
- For direct copy, ensure headers are in your include path

**"Undefined reference to clog symbols"**
- Ensure you're linking: `target_link_libraries(your_target clog::clog)`
- CLog is header-only, no separate compilation needed

**Arduino IDE compilation errors**
- Ensure headers are in `libraries/clog/src/` directory
- Include full path: `#include <clog/log.hpp>`
- Set log level before include: `#define CLOG_LEVEL 3`

### Getting Help

If you encounter issues:

1. Check the [examples](../examples/) directory for working configurations
2. Verify your CMake version is 3.12 or later
3. Ensure you're using a supported C++17 compiler
4. See [troubleshooting section](configuration.md#troubleshooting) in configuration guide

## Next Steps

- **[Quick Start](quick-start.md)** - Get up and running quickly
- **[Configuration](configuration.md)** - Configure CLog for your needs
- **[Platform Support](platform-support.md)** - Platform-specific setup guides