# Platform Support Guide

CLog provides cross-platform compatibility across embedded systems and desktop platforms. This guide covers platform-specific setup, configuration, and best practices.

## Supported Platforms

### Desktop Platforms
- **Windows** - Full feature support with colored output
- **Linux** - Full feature support with colored output  
- **macOS** - Full feature support with colored output

### Embedded Platforms
- **Arduino** - All Arduino-compatible boards
- **ESP32** - Both Arduino framework and ESP-IDF
- **ESP8266** - Arduino framework support
- **Raspberry Pi Pico (RP2040)** - Arduino framework and Pico SDK

## Platform Detection

CLog uses simplified platform detection that defaults to desktop behavior. For embedded platforms, explicit configuration is recommended for predictable results.

### Automatic Detection (Default)

```cpp
#include <clog/log.hpp>  // Uses automatic platform detection

int main() {
    // Desktop behavior by default
    CLOG_INFO("App", "Using automatic platform detection");
    return 0;
}
```

### Explicit Configuration (Recommended)

#### CMake Configuration

```cmake
# Set platform at build time via CMake (recommended)
set(CLOG_PLATFORM "RP2040_SDK" CACHE STRING "CLog target platform")

# Or pass as command-line argument
# cmake -DCLOG_PLATFORM=ESP32 ..

# Available platform values:
# CLOG_AUTO_DETECT (default), CLOG_ARDUINO, CLOG_ESP32, CLOG_ESP8266, 
# CLOG_RP2040_ARDUINO, CLOG_RP2040_SDK, CLOG_ESP_IDF,
# CLOG_DESKTOP, CLOG_WINDOWS, CLOG_LINUX, CLOG_MACOS
```

#### Runtime Configuration

```cpp
// Available platforms (for runtime configuration)
enum class Platform {
    CLOG_ARDUINO, CLOG_ESP32, CLOG_ESP8266, CLOG_RP2040_ARDUINO, CLOG_RP2040_SDK,
    CLOG_ESP_IDF, CLOG_DESKTOP, CLOG_WINDOWS, CLOG_LINUX, CLOG_MACOS, CLOG_AUTO_DETECT
};

// Explicit platform configuration (when CMake config not available)
clogger::Logger::init(clogger::Platform::CLOG_RP2040_SDK);

// Or set platform separately
clogger::Logger::setPlatform(clogger::Platform::CLOG_RP2040_SDK);
clogger::Logger::init();

// Query current platform
clogger::Platform current = clogger::Logger::getPlatform();
```

## Platform-Specific Setup

### Desktop Development

Desktop platforms provide the full CLog feature set with optimal development experience.

**Features:**
- Colored console output with ANSI escape codes
- Large buffer sizes (default 512 bytes)
- Full API support including assertions
- std::cout output (faster than printf)

**Configuration:**

```cmake
add_executable(desktop_app main.cpp)
target_compile_definitions(desktop_app PRIVATE
    CLOG_LEVEL=5                    # TRACE level for development
    CLOG_PLATFORM_DESKTOP=1         # Explicit desktop platform
    CLOG_BUFFER_SIZE=1024           # Large buffer
    CLOG_HAS_COLOR_SUPPORT=1        # Enable colors
    CLOG_HAS_PRINTF_SUPPORT=0       # Use std::cout
)
target_link_libraries(desktop_app PRIVATE clog::clog)
```

**Usage:**

```cpp
#include <clog/log.hpp>

int main() {
    // Configure colors for better visual distinction
    clogger::Logger::setTagColor("Database", clogger::Color::CLOG_BRIGHT_CYAN);
    clogger::Logger::setTagColor("Network", clogger::Color::CLOG_BRIGHT_MAGENTA);
    
    // Full feature set available
    CLOG_INFO("App", "Desktop application started");
    CLOG_DEBUG("Database", "This appears in cyan");
    CLOG_WARN("Network", "This appears in magenta");
    
    return 0;
}
```

### Arduino Boards

Arduino-compatible boards use Serial output with optimized settings for memory-constrained environments.

**Features:**
- Serial.print() output
- Smaller buffer sizes (default 256 bytes)
- No color support (saves memory)
- Compile-time optimization for minimal footprint

**Arduino IDE Setup:**

```cpp
// In your Arduino sketch
#define CLOG_LEVEL 3               // INFO level for Arduino
#define CLOG_BUFFER_SIZE 256       // Smaller buffer for embedded
#define CLOG_PLATFORM_ARDUINO      // Explicit platform
#include <clog/log.hpp>

void setup() {
    Serial.begin(115200);
    CLOG_INFO("Arduino", "Sketch started");
    
    // Configure for minimal memory usage
    clogger::Logger::setLevel(clogger::Level::INFO);
}

void loop() {
    static int counter = 0;
    CLOG_DEBUG("Loop", "Loop iteration: %d", counter++);
    delay(1000);
}
```

**PlatformIO Setup:**

```ini
[env:arduino_uno]
platform = atmelavr
board = uno
framework = arduino

lib_deps = 
    file://path/to/clog

build_flags =
    -DCLOG_LEVEL=3
    -DCLOG_BUFFER_SIZE=128        # Even smaller for Uno
    -DCLOG_PLATFORM_ARDUINO=1
    -DCLOG_HAS_COLOR_SUPPORT=0
    -DCLOG_HAS_PRINTF_SUPPORT=1
```

### ESP32 Development

ESP32 supports both Arduino framework and ESP-IDF with platform-specific optimizations.

#### ESP32 with Arduino Framework

**Features:**
- Serial.print() output
- Medium buffer sizes (default 256 bytes)
- WiFi and multi-core logging support
- FreeRTOS task-safe operation

**Configuration:**

```cmake
add_executable(esp32_app main.cpp)
target_compile_definitions(esp32_app PRIVATE
    CLOG_LEVEL=3                   # INFO level for production
    CLOG_PLATFORM_ESP32=1          # Explicit ESP32 platform
    CLOG_BUFFER_SIZE=512           # Medium buffer
    CLOG_HAS_COLOR_SUPPORT=0       # No colors for embedded
    CLOG_HAS_PRINTF_SUPPORT=1      # Use Serial.print
)
target_link_libraries(esp32_app PRIVATE clog::clog)
```

**Usage:**

```cpp
#include <clog/log.hpp>
#include <WiFi.h>

void setup() {
    Serial.begin(115200);
    
    CLOG_INFO("ESP32", "System initializing");
    
    // WiFi setup with logging
    WiFi.begin("SSID", "password");
    while (WiFi.status() != WL_CONNECTED) {
        CLOG_DEBUG("WiFi", "Connecting...");
        delay(1000);
    }
    
    CLOG_INFO("WiFi", "Connected with IP: %s", WiFi.localIP().toString().c_str());
}

void loop() {
    // Your main application with logging
    CLOG_TRACE("Loop", "Main loop executing");
    delay(10000);
}
```

#### ESP32 with ESP-IDF

**Features:**
- printf() output to console
- Component-based logging integration
- Advanced debugging features
- Flash and RAM optimizations

**Configuration:**

```cmake
# In ESP-IDF CMakeLists.txt
idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS "."
)

target_compile_definitions(${COMPONENT_LIB} PRIVATE
    CLOG_LEVEL=3
    CLOG_PLATFORM_ESP_IDF=1
    CLOG_BUFFER_SIZE=512
    CLOG_HAS_PRINTF_SUPPORT=1
)

# Add CLog
add_subdirectory(components/clog)
target_link_libraries(${COMPONENT_LIB} PRIVATE clog::clog)
```

**Usage:**

```cpp
#include <clog/log.hpp>
#include "esp_log.h"

extern "C" void app_main() {
    CLOG_INFO("ESP-IDF", "Application starting");
    
    // CLog can coexist with ESP-IDF logging
    ESP_LOGI("ESP_TAG", "ESP-IDF log message");
    CLOG_INFO("CLOG_TAG", "CLog message");
    
    // Your application logic
    while (1) {
        CLOG_DEBUG("Main", "Main loop iteration");
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
```

### Raspberry Pi Pico (RP2040)

RP2040 supports both Arduino framework and native Pico SDK with optimized configurations.

#### RP2040 with Arduino Framework

**Configuration:**

```cmake
add_executable(pico_arduino_app main.cpp)
target_compile_definitions(pico_arduino_app PRIVATE
    CLOG_LEVEL=4                   # DEBUG level
    CLOG_PLATFORM_RP2040_ARDUINO=1 # Arduino framework
    CLOG_BUFFER_SIZE=512           # Medium buffer
    CLOG_HAS_PRINTF_SUPPORT=1      # Use Serial.print
)
target_link_libraries(pico_arduino_app PRIVATE clog::clog)
```

#### RP2040 with Pico SDK

**Features:**
- printf() output to USB/UART
- SDK-specific optimizations
- Real-time performance
- Minimal memory footprint

**Configuration:**

```cmake
# Pico SDK CMakeLists.txt
add_executable(pico_app main.cpp)
target_compile_definitions(pico_app PRIVATE 
    CLOG_LEVEL=4                   # DEBUG level
    CLOG_PLATFORM_RP2040_SDK=1     # Pico SDK platform
    CLOG_BUFFER_SIZE=512           # Medium buffer
    CLOG_HAS_COLOR_SUPPORT=0       # No colors
    CLOG_HAS_PRINTF_SUPPORT=1      # Use printf
)
target_link_libraries(pico_app PRIVATE clog::clog pico_stdlib)

# Enable USB output
pico_enable_stdio_usb(pico_app 1)
pico_enable_stdio_uart(pico_app 0)
```

**Usage:**

```cpp
#include <clog/log.hpp>
#include "pico/stdlib.h"

int main() {
    stdio_init_all();
    
    CLOG_INFO("Pico", "RP2040 application started");
    
    while (true) {
        CLOG_DEBUG("Loop", "Main loop executing");
        sleep_ms(1000);
    }
    
    return 0;
}
```

## Platform Feature Matrix

| Feature | Desktop | Arduino | ESP32 | ESP8266 | RP2040 |
|---------|---------|---------|-------|---------|--------|
| Colored Output | ✅ | ❌ | ❌ | ❌ | ❌ |
| Large Buffers | ✅ | ❌ | ⚠️ | ❌ | ⚠️ |
| Tag Filtering | ✅ | ✅ | ✅ | ⚠️ | ✅ |
| Library Tagging | ✅ | ✅ | ✅ | ⚠️ | ✅ |
| Assertions | ✅ | ✅ | ✅ | ✅ | ✅ |
| Callbacks | ✅ | ✅ | ✅ | ✅ | ✅ |
| Thread Safety | ✅ | N/A | ✅ | N/A | ✅ |

**Legend:**
- ✅ Full support
- ⚠️ Limited support (memory constraints)
- ❌ Not supported (platform limitations)

## Platform-Specific Best Practices

### Desktop Development
- Use maximum log level (TRACE) during development
- Enable colors for better visual distinction
- Use large buffers (1024+ bytes) for complex messages
- Leverage full API including assertions

### Embedded Development
- Use lower log levels (INFO/WARN) in production
- Minimize buffer sizes to conserve RAM
- Disable colors and assertions in production builds
- Use compile-time filtering for optimal performance

### Memory-Constrained Platforms
- Set `CLOG_LEVEL=1` or `CLOG_LEVEL=2` for production
- Use `CLOG_BUFFER_SIZE=128` or smaller
- Disable tag filtering if not needed
- Compile out unused features

## Platform Selection Guide

**Choose AUTO_DETECT when:**
- Rapid prototyping on desktop
- Platform detection works reliably
- No specific optimizations needed

**Choose explicit platform when:**
- Production embedded builds
- Cross-compilation scenarios  
- Platform-specific optimizations required
- Avoiding runtime detection overhead

## Troubleshooting Platform Issues

**Issue: Colors not working on embedded platform**
- **Cause**: Embedded platforms don't support ANSI escape codes
- **Solution**: Colors are automatically disabled on embedded platforms

**Issue: Buffer overflow on small microcontrollers**
- **Cause**: Default buffer size too large for available RAM
- **Solution**: Reduce `CLOG_BUFFER_SIZE` to 128 or 64 bytes

**Issue: Compilation errors on ESP-IDF**
- **Cause**: Platform not detected correctly
- **Solution**: Set `CLOG_PLATFORM_ESP_IDF=1` explicitly

**Issue: Serial output not appearing on Arduino**
- **Cause**: Serial not initialized or wrong baud rate
- **Solution**: Call `Serial.begin(115200)` in setup()

## Next Steps

- **[Configuration](configuration.md)** - Detailed configuration options
- **[Multi-Library Projects](multi-library-projects.md)** - Cross-platform library development
- **[Performance](performance.md)** - Platform-specific optimization guides