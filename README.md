# CLog - Unified Logging Library

A unified header-only C++ logging library with tag-based filtering, library identification, and seamless embedded-to-desktop portability.

## Quick Start

```cpp
#include <clog/log.hpp>

int main() {
    clogger::Logger::setLevel(clogger::Level::DEBUG);
    CLOG_INFO("MyApp", "Hello, CLog!");
    return 0;
}
```

## Documentation

- [Complete Documentation](docs/README.md)
- [Changelog](docs/CHANGELOG.md)

## Features

- Header-only, cross-platform (Arduino, ESP32, RP2040, desktop)
- Five log levels with compile-time filtering
- Callback support for integration with parent applications
- Zero dependencies beyond standard C++ library

## License

MIT License - see [LICENSE](LICENSE) for details.