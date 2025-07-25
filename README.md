# CLog - Unified Logging Library

A unified header-only C++ logging library with tag-based filtering, library identification, and seamless embedded-to-desktop portability.

## Quick Start

```cpp
// Configure log level at compile time (recommended)
#define CLOG_LEVEL 4  // DEBUG level
#include <clog/log.hpp>

int main() {
    // No setup needed - log level configured at compile time
    CLOG_INFO("MyApp", "Hello, CLog!");
    return 0;
}
```

### CMake Integration (Recommended)

```cmake
# Set log level via CMake for better performance
add_executable(my_app main.cpp)
target_compile_definitions(my_app PRIVATE CLOG_LEVEL=4)  # DEBUG level
target_link_libraries(my_app PRIVATE clog::clog)
```

## Documentation

- [Complete Documentation](docs/README.md)
- [Changelog](docs/CHANGELOG.md)

## Features

- **Header-only**: Single include, no compilation required
- **Cross-platform**: Arduino, ESP32, RP2040, and desktop with simplified detection
- **Compile-time log filtering**: Better performance through compile-time level elimination
- **Tag-based filtering**: Granular control with whitelist/blacklist modes
- **Library identification**: Automatic nested dependency tracking
- **CMake integration**: Platform and configuration via build system
- **Callback support**: Integration with parent application logging systems
- **Zero dependencies**: Only standard C++ library features

## License

MIT License - see [LICENSE](LICENSE) for details.