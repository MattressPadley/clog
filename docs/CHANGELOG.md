# Changelog

# 0.1.4 (2025-08-08)

Looking at these commits, I'll format them into a clean changelog:

## Bug Fixes
- Fixed macro naming conflicts with Arduino platform

## Improvements
- Updated documentation to reflect new CLOG_ prefixed enum API


# 0.1.3 (2025-07-25)

## Improvements
- Restructured documentation into focused, well-organized files for better navigation
- Added comprehensive best practices guide for log level hierarchy management


# 0.1.2 (2025-07-25)

## Features
- Added compile-time log level configuration for better performance control

## Bug Fixes
- Fixed buffer size consistency and code quality issues

## Improvements
- Simplified platform detection system
- Enhanced README to better highlight key differentiating features

## Other
- Updated documentation to reflect compile-time configuration changes


# 0.1.1 (2025-07-25)

## Features
- Added nested submodule support with comprehensive documentation
- Added compile-time platform configuration via CMake

## Bug Fixes
- Fixed AUTO_DETECT fallback logic for platform detection

## Improvements
- Enhanced CMake platform configuration with comprehensive integration tests
- Updated documentation to recommend CMake configuration for library naming


# 0.1.0 (2025-07-24)

## Features
- Add runtime platform configuration to eliminate build warnings
- Add comprehensive test suite for nested library system

## Bug Fixes
- Fix library tagging global state issue preventing correct library identification
- Add explicit test for nested library tag visibility behavior

## Improvements
- **BREAKING CHANGE**: Switch from runtime to compile-time library identification for better performance
- Update documentation for new compile-time library identification system
- Remove unimplemented configuration flags and clean up codebase

## Other
- Claude Code Review workflow
- Claude PR Assistant workflow


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