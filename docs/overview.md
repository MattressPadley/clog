# CLog - Unified Logging Library

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

## Design Philosophy

CLog is built around several key principles:

1. **Compile-time Performance**: Log levels can be filtered out at compile time, generating zero code for unused levels
2. **Embedded-First Design**: Fixed buffer sizes, minimal memory footprint, and embedded platform optimizations
3. **Desktop Development Friendly**: Rich features like colors, extensive API, and debugging tools for development
4. **Integration Focused**: Designed to work seamlessly as a dependency in larger projects
5. **Unified API**: Same logging calls work identically across all supported platforms

## Architecture Overview

### Core Components

- **`include/clog/log.hpp`** - Main header containing complete logging implementation
- **`include/clog/platform.hpp`** - Platform detection and abstraction utilities  
- **`include/clog/config.hpp`** - Compile-time configuration options

### Key Architecture Patterns

1. **Single Header Design**: Complete implementation in `log.hpp` for easy integration
2. **Platform Abstraction**: Automatic detection of Arduino, ESP32, RP2040, and desktop platforms
3. **Callback Integration**: Support for routing logs to parent application logging systems
4. **Dual Filtering System**: Both log level and tag-based filtering for granular control
5. **Library Tagging System**: Automatic nested library identification with parent-controlled visibility
6. **Compile-time Filtering**: Log levels can be filtered out at compile time for performance
7. **Fixed Buffer Sizes**: Uses fixed-size buffers (default 512 bytes) for embedded compatibility

### Log Levels

- `OFF` (0) - No logging
- `ERROR` (1) - Error messages only  
- `WARN` (2) - Warnings and errors
- `INFO` (3) - Informational messages and above (default)
- `DEBUG` (4) - Debug messages and above
- `TRACE` (5) - All messages including trace

### Platform Support

- **Desktop**: Windows, Linux, macOS with colored console output
- **Arduino**: All Arduino-compatible boards with Serial output
- **ESP32/ESP8266**: Both Arduino framework and ESP-IDF
- **RP2040**: Arduino framework and Pico SDK support

## Integration with Slide Kick

CLog is designed as a unified logging solution for the Slide Kick project, providing consistent logging across:

- ESP32 main controller firmware
- RP2040 subsystem firmware
- sk-slide motorized fader library
- sk-oal OSC abstraction layer
- Desktop tools and applications

## License

MIT License - see [LICENSE](../LICENSE) file for details.