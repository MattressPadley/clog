# CLog - Unified Logging Library

A lightweight, header-only C++ logging library for embedded and desktop platforms.

## Quick Start

```cpp
#include <clog/log.hpp>

int main() {
    clog::Logger::setLevel(clog::Level::DEBUG);
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