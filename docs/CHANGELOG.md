# Changelog

All notable changes to the CLog library will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.0.1] - 2024-12-19

### Added
- Initial release of CLog unified logging library
- Cross-platform support for Arduino, ESP32, RP2040, and desktop platforms
- Header-only implementation with zero dependencies
- Five log levels: ERROR, WARN, INFO, DEBUG, TRACE
- Callback support for parent application integration
- Compile-time log level filtering for performance optimization
- Platform-specific output formatting (colors on desktop, Arduino Serial support)
- CMake integration with modern CMake practices
- PlatformIO library manifest for embedded development
- Comprehensive examples:
  - Desktop example with colored console output
  - Arduino basic example with sensor simulation
  - ESP32 advanced example with WiFi and FreeRTOS tasks
  - Callback integration example for parent applications
- Platform detection utilities in `platform.hpp`
- Compile-time configuration options in `config.hpp`
- MIT license for open source usage
- Complete documentation and usage examples

### Technical Details
- C++17 standard requirement
- Fixed-size buffers for embedded compatibility
- Static assertions for compile-time validation
- Platform-specific optimizations for memory usage
- Support for printf-style formatted logging
- Thread-safe design considerations (implementation pending)

[0.0.1]: https://github.com/MattressPadley/clog/releases/tag/v0.0.1