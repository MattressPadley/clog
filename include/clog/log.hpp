#pragma once
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include "config.hpp"

// Platform detection handled by platform.hpp - include it for platform defines
#include "platform.hpp"

namespace clogger {

enum class Level {
    OFF = 0,
    ERROR = 1,
    WARN = 2, 
    INFO = 3,
    DEBUG = 4,
    TRACE = 5
};

enum class Color {
    DEFAULT = 0,
    BLACK,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
    BRIGHT_BLACK,
    BRIGHT_RED,
    BRIGHT_GREEN,
    BRIGHT_YELLOW,
    BRIGHT_BLUE,
    BRIGHT_MAGENTA,
    BRIGHT_CYAN,
    BRIGHT_WHITE
};

enum class Platform {
    ARDUINO,
    ESP32,
    ESP8266,
    RP2040_ARDUINO,
    RP2040_SDK,
    ESP_IDF,
    DESKTOP,
    WINDOWS,
    LINUX,
    MACOS,
    AUTO_DETECT  // Default: attempt automatic detection
};

class Logger {
public:
    // Core API
    static void log(Level level, const char* tag, const char* format, ...);
    static void setLevel(Level level);
    static Level getLevel();
    
    // Callback integration for parent applications
    using Callback = void(*)(Level level, const char* tag, const char* message, const char* libraryName);
    static void setCallback(Callback callback);
    static void enableDirectOutput(bool enabled = true);
    
    // Platform-specific initialization (optional)
    static void init();
    static void init(Platform platform);
    
    // Platform configuration
    static void setPlatform(Platform platform);
    static Platform getPlatform();
    
    // Platform detection helpers
    static bool isArduinoPlatform();
    static bool isDesktopPlatform();
    static bool isEmbeddedPlatform();
    static bool hasColorSupport();
    static bool hasPrintfSupport();
    
    // Tag color configuration
    static void setTagColor(const char* tag, Color color);
    static void clearTagColor(const char* tag);
    static void clearAllTagColors();
    
    // Library identification and control (compile-time only)
    static void enableLibraryTags(bool enabled = true);
    static bool isLibraryTagsEnabled();
    
    // Library color configuration
    static void setLibraryColor(const char* library, Color color);
    static void clearLibraryColor(const char* library);
    static void clearAllLibraryColors();
    
    // Tag filtering configuration
    static void enableTag(const char* tag);
    static void disableTag(const char* tag);
    static void enableAllTags();
    static void disableAllTags();
    static bool isTagEnabled(const char* tag);
    static void clearTagFilters();
    
    // Library-aware convenience methods (compile-time library identification)
    static void error_with_library(const char* tag, const char* libraryName, const char* format, ...);
    static void warn_with_library(const char* tag, const char* libraryName, const char* format, ...);
    static void info_with_library(const char* tag, const char* libraryName, const char* format, ...);
    static void debug_with_library(const char* tag, const char* libraryName, const char* format, ...);
    static void trace_with_library(const char* tag, const char* libraryName, const char* format, ...);
    
private:
    static Level currentLevel;
    static Callback logCallback;
    static bool directOutput;
    
    // Platform configuration
    static Platform currentPlatform;
    
    // Tag color storage
    static constexpr size_t MAX_TAG_COLORS = 32;
    struct TagColor {
        char tag[32];
        Color color;
        bool active;
    };
    static TagColor tagColors[MAX_TAG_COLORS];
    
    // Library context storage
    static bool libraryTagsEnabled;
    
    // Library color storage
    static constexpr size_t MAX_LIBRARY_COLORS = config::MAX_LIBRARY_COLORS;
    struct LibraryColor {
        char library[config::MAX_LIBRARY_NAME_LENGTH + 1];
        Color color;
        bool active;
    };
    static LibraryColor libraryColors[MAX_LIBRARY_COLORS];
    
    // Tag filtering storage
#if CLOG_ENABLE_TAG_FILTERING
    enum class TagFilterMode {
        ALLOW_ALL,      // Default: allow all tags (no filtering)
        WHITELIST,      // Only allow explicitly enabled tags
        BLACKLIST       // Allow all except explicitly disabled tags
    };
    
    struct TagFilter {
        char tag[32];
        bool active;
    };
    static TagFilter tagFilters[CLOG_MAX_TAG_FILTERS];
    static TagFilterMode filterMode;
#endif
    
    static void output(Level level, const char* tag, const char* message, const char* libraryName);
    static const char* levelToString(Level level);
    static const char* levelToColor(Level level);
    static const char* colorToAnsi(Color color);
    static Color getTagColor(const char* tag);
    static Color getLibraryColor(const char* library);
    
    // Tag filtering helper methods
#if CLOG_ENABLE_TAG_FILTERING
    static bool checkTagFilter(const char* tag);
    static int findTagFilter(const char* tag);
#endif
};

} // namespace clogger

// Convenience macros for easy usage
// These macros allow libraries to define CLOG_LIBRARY_NAME to get persistent library identification
#ifndef CLOG_LIBRARY_NAME
    #define CLOG_LIBRARY_NAME nullptr
#endif

#define CLOG_ERROR(tag, fmt, ...)   clogger::Logger::error_with_library(tag, CLOG_LIBRARY_NAME, fmt, ##__VA_ARGS__)
#define CLOG_WARN(tag, fmt, ...)    clogger::Logger::warn_with_library(tag, CLOG_LIBRARY_NAME, fmt, ##__VA_ARGS__)
#define CLOG_INFO(tag, fmt, ...)    clogger::Logger::info_with_library(tag, CLOG_LIBRARY_NAME, fmt, ##__VA_ARGS__)
#define CLOG_DEBUG(tag, fmt, ...)   clogger::Logger::debug_with_library(tag, CLOG_LIBRARY_NAME, fmt, ##__VA_ARGS__)
#define CLOG_TRACE(tag, fmt, ...)   clogger::Logger::trace_with_library(tag, CLOG_LIBRARY_NAME, fmt, ##__VA_ARGS__)

// Compile-time level filtering
#ifndef CLOG_LEVEL
    #define CLOG_LEVEL 3  // Default to INFO
#endif

#if CLOG_LEVEL < 1
    #undef CLOG_ERROR
    #define CLOG_ERROR(tag, fmt, ...) ((void)0)
#endif
#if CLOG_LEVEL < 2
    #undef CLOG_WARN
    #define CLOG_WARN(tag, fmt, ...) ((void)0)
#endif
#if CLOG_LEVEL < 3
    #undef CLOG_INFO
    #define CLOG_INFO(tag, fmt, ...) ((void)0)
#endif
#if CLOG_LEVEL < 4
    #undef CLOG_DEBUG
    #define CLOG_DEBUG(tag, fmt, ...) ((void)0)
#endif
#if CLOG_LEVEL < 5
    #undef CLOG_TRACE
    #define CLOG_TRACE(tag, fmt, ...) ((void)0)
#endif

// Implementation (header-only)
namespace clogger {

// Static member definitions
inline Level Logger::currentLevel = Level::INFO;
inline Logger::Callback Logger::logCallback = nullptr;
inline bool Logger::directOutput = true;
inline Platform Logger::currentPlatform = Platform::DESKTOP;
inline Logger::TagColor Logger::tagColors[MAX_TAG_COLORS] = {};

// Library context static members
inline bool Logger::libraryTagsEnabled = false;
inline Logger::LibraryColor Logger::libraryColors[MAX_LIBRARY_COLORS] = {};

#if CLOG_ENABLE_TAG_FILTERING
inline Logger::TagFilter Logger::tagFilters[CLOG_MAX_TAG_FILTERS] = {};
inline Logger::TagFilterMode Logger::filterMode = Logger::TagFilterMode::ALLOW_ALL;
#endif

inline void Logger::log(Level level, const char* tag, const char* format, ...) {
    if (level > currentLevel) return;
    
#if CLOG_ENABLE_TAG_FILTERING
    if (!checkTagFilter(tag)) return;
#endif
    
    char buffer[512];  // Fixed size for embedded compatibility
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    output(level, tag, buffer, nullptr);
}

inline void Logger::output(Level level, const char* tag, const char* message, const char* libraryName) {
    if (logCallback) {
        logCallback(level, tag, message, libraryName);
    } else if (directOutput) {
        const char* levelStr = levelToString(level);
        
        if (isArduinoPlatform()) {
            // Arduino platform: use Serial.printf
            #ifdef ARDUINO
            Serial.printf("[%s] ", levelStr);
            if (libraryTagsEnabled && libraryName && libraryName[0] != '\0') {
                Serial.printf("[%s]", libraryName);
            }
            Serial.printf("[%s]: %s\n", tag, message);
            #endif
        } else if (isDesktopPlatform()) {
            // Desktop platform: use std::cout with colors
            #if defined(_WIN32) || defined(__linux__) || defined(__APPLE__)
            const char* levelColor = levelToColor(level);
            Color tagColor = getTagColor(tag);
            const char* tagColorCode = colorToAnsi(tagColor);
            
            std::cout << "[" << levelColor << levelStr << "\033[0m" << "] ";
            
            // Library tag if enabled and set
            if (libraryTagsEnabled && libraryName && libraryName[0] != '\0') {
                Color libraryColor = getLibraryColor(libraryName);
                const char* libraryColorCode = colorToAnsi(libraryColor);
                std::cout << "[";
                if (libraryColor != Color::DEFAULT) {
                    std::cout << libraryColorCode << libraryName << "\033[0m";
                } else {
                    std::cout << libraryName;
                }
                std::cout << "]";
            }
            
            // Regular tag (always in brackets now)
            std::cout << "[";
            if (tagColor != Color::DEFAULT) {
                std::cout << tagColorCode << tag << "\033[0m";
            } else {
                std::cout << tag;
            }
            std::cout << "]: " << message << std::endl;
            #endif
        } else {
            // Fallback: use printf
            printf("[%s] ", levelStr);
            if (libraryTagsEnabled && libraryName && libraryName[0] != '\0') {
                printf("[%s]", libraryName);
            }
            printf("[%s]: %s\n", tag, message);
        }
    }
}

inline void Logger::setCallback(Callback callback) {
    logCallback = callback;
    directOutput = (callback == nullptr);
}

inline const char* Logger::levelToString(Level level) {
    switch (level) {
        case Level::ERROR: return "ERROR";
        case Level::WARN:  return "WARN ";
        case Level::INFO:  return "INFO ";
        case Level::DEBUG: return "DEBUG";
        case Level::TRACE: return "TRACE";
        default: return "?????";
    }
}

inline const char* Logger::levelToColor(Level level) {
    switch (level) {
        case Level::ERROR: return "\033[91m";  // Bright red
        case Level::WARN:  return "\033[93m";  // Bright yellow
        case Level::INFO:  return "\033[92m";  // Bright green
        case Level::DEBUG: return "\033[94m";  // Bright blue
        case Level::TRACE: return "\033[90m";  // Dark gray
        default: return "\033[0m";
    }
}

// Convenience method implementations
// Library-aware convenience methods (pure compile-time library identification)
inline void Logger::error_with_library(const char* tag, const char* libraryName, const char* format, ...) {
    if (Level::ERROR > currentLevel) return;
#if CLOG_ENABLE_TAG_FILTERING
    if (!checkTagFilter(tag)) return;
#endif
    va_list args;
    va_start(args, format);
    char buffer[512];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    output(Level::ERROR, tag, buffer, libraryName);
}

inline void Logger::warn_with_library(const char* tag, const char* libraryName, const char* format, ...) {
    if (Level::WARN > currentLevel) return;
#if CLOG_ENABLE_TAG_FILTERING
    if (!checkTagFilter(tag)) return;
#endif
    va_list args;
    va_start(args, format);
    char buffer[512];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    output(Level::WARN, tag, buffer, libraryName);
}

inline void Logger::info_with_library(const char* tag, const char* libraryName, const char* format, ...) {
    if (Level::INFO > currentLevel) return;
#if CLOG_ENABLE_TAG_FILTERING
    if (!checkTagFilter(tag)) return;
#endif
    va_list args;
    va_start(args, format);
    char buffer[512];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    output(Level::INFO, tag, buffer, libraryName);
}

inline void Logger::debug_with_library(const char* tag, const char* libraryName, const char* format, ...) {
    if (Level::DEBUG > currentLevel) return;
#if CLOG_ENABLE_TAG_FILTERING
    if (!checkTagFilter(tag)) return;
#endif
    va_list args;
    va_start(args, format);
    char buffer[512];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    output(Level::DEBUG, tag, buffer, libraryName);
}

inline void Logger::trace_with_library(const char* tag, const char* libraryName, const char* format, ...) {
    if (Level::TRACE > currentLevel) return;
#if CLOG_ENABLE_TAG_FILTERING
    if (!checkTagFilter(tag)) return;
#endif
    va_list args;
    va_start(args, format);
    char buffer[512];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    output(Level::TRACE, tag, buffer, libraryName);
}

inline void Logger::setLevel(Level level) { currentLevel = level; }
inline Level Logger::getLevel() { return currentLevel; }
inline void Logger::enableDirectOutput(bool enabled) { directOutput = enabled; }

// Platform configuration methods
inline void Logger::setPlatform(Platform platform) { currentPlatform = platform; }
inline Platform Logger::getPlatform() { return currentPlatform; }

// Platform detection helpers
inline bool Logger::isArduinoPlatform() {
    // Simplified: only true for Arduino-style platforms (those using Serial.printf)
    #if defined(CLOG_PLATFORM_ARDUINO) || defined(CLOG_PLATFORM_ESP32) || defined(CLOG_PLATFORM_ESP8266) || defined(CLOG_PLATFORM_AVR) || defined(CLOG_PLATFORM_RP2040_ARDUINO)
        return true;
    #else
        return false;  // Default to false (desktop behavior)
    #endif
}

inline bool Logger::isDesktopPlatform() {
    // Simplified: false only for explicitly defined embedded platforms, otherwise default to true
    #if defined(CLOG_PLATFORM_ARDUINO) || defined(CLOG_PLATFORM_ESP32) || defined(CLOG_PLATFORM_ESP8266) || defined(CLOG_PLATFORM_AVR) || defined(CLOG_PLATFORM_RP2040_ARDUINO) || defined(CLOG_PLATFORM_RP2040_SDK) || defined(CLOG_PLATFORM_ESP_IDF)
        return false;
    #else
        return true;  // Default to true (desktop behavior)
    #endif
}

inline bool Logger::isEmbeddedPlatform() {
    // Simplified: true only for explicitly defined embedded platforms, otherwise default to false
    #if defined(CLOG_PLATFORM_ARDUINO) || defined(CLOG_PLATFORM_ESP32) || defined(CLOG_PLATFORM_ESP8266) || defined(CLOG_PLATFORM_AVR) || defined(CLOG_PLATFORM_RP2040_ARDUINO) || defined(CLOG_PLATFORM_RP2040_SDK) || defined(CLOG_PLATFORM_ESP_IDF)
        return true;
    #else
        return false;  // Default to false (desktop behavior)
    #endif
}

inline bool Logger::hasColorSupport() {
    // Simplified: use platform.hpp define (defaults to true for desktop)
    return CLOG_HAS_COLOR_SUPPORT;
}

inline bool Logger::hasPrintfSupport() {
    // Simplified: use platform.hpp define (defaults to false for desktop)
    return CLOG_HAS_PRINTF_SUPPORT;
}

inline void Logger::init() {
    // Platform-specific initialization: CMake config → runtime platform → auto-detection
    #if defined(CLOG_PLATFORM_RP2040_SDK)
        // RP2040 SDK requires stdio initialization
        #if defined(PICO_SDK_VERSION_MAJOR)
        stdio_init_all();
        #endif
    #elif !defined(CLOG_PLATFORM_ARDUINO) && !defined(CLOG_PLATFORM_ESP32) && !defined(CLOG_PLATFORM_ESP8266) && !defined(CLOG_PLATFORM_RP2040_ARDUINO) && !defined(CLOG_PLATFORM_ESP_IDF) && !defined(CLOG_PLATFORM_DESKTOP) && !defined(CLOG_PLATFORM_WINDOWS) && !defined(CLOG_PLATFORM_LINUX) && !defined(CLOG_PLATFORM_MACOS)
        // Fall back to runtime platform configuration
        if (currentPlatform == Platform::RP2040_SDK) {
            // RP2040 SDK requires stdio initialization
            #if defined(PICO_SDK_VERSION_MAJOR)
            stdio_init_all();
            #endif
        }
    #endif
    // Arduino platforms: Serial already initialized by Arduino framework
    // Desktop platforms: std::cout/printf ready by default
    // ESP-IDF: logging already initialized
}

inline void Logger::init(Platform platform) {
    setPlatform(platform);
    init();
}

// Tag color management
inline void Logger::setTagColor(const char* tag, Color color) {
    // First check if tag already exists
    for (size_t i = 0; i < MAX_TAG_COLORS; i++) {
        if (tagColors[i].active && strcmp(tagColors[i].tag, tag) == 0) {
            tagColors[i].color = color;
            return;
        }
    }
    
    // Find empty slot
    for (size_t i = 0; i < MAX_TAG_COLORS; i++) {
        if (!tagColors[i].active) {
            strncpy(tagColors[i].tag, tag, sizeof(tagColors[i].tag) - 1);
            tagColors[i].tag[sizeof(tagColors[i].tag) - 1] = '\0';
            tagColors[i].color = color;
            tagColors[i].active = true;
            return;
        }
    }
}

inline void Logger::clearTagColor(const char* tag) {
    for (size_t i = 0; i < MAX_TAG_COLORS; i++) {
        if (tagColors[i].active && strcmp(tagColors[i].tag, tag) == 0) {
            tagColors[i].active = false;
            return;
        }
    }
}

inline void Logger::clearAllTagColors() {
    for (size_t i = 0; i < MAX_TAG_COLORS; i++) {
        tagColors[i].active = false;
    }
}

inline Color Logger::getTagColor(const char* tag) {
    for (size_t i = 0; i < MAX_TAG_COLORS; i++) {
        if (tagColors[i].active && strcmp(tagColors[i].tag, tag) == 0) {
            return tagColors[i].color;
        }
    }
    return Color::DEFAULT;
}

inline const char* Logger::colorToAnsi(Color color) {
    switch (color) {
        case Color::DEFAULT:      return "";
        case Color::BLACK:        return "\033[30m";
        case Color::RED:          return "\033[31m";
        case Color::GREEN:        return "\033[32m";
        case Color::YELLOW:       return "\033[33m";
        case Color::BLUE:         return "\033[34m";
        case Color::MAGENTA:      return "\033[35m";
        case Color::CYAN:         return "\033[36m";
        case Color::WHITE:        return "\033[37m";
        case Color::BRIGHT_BLACK: return "\033[90m";
        case Color::BRIGHT_RED:   return "\033[91m";
        case Color::BRIGHT_GREEN: return "\033[92m";
        case Color::BRIGHT_YELLOW:return "\033[93m";
        case Color::BRIGHT_BLUE:  return "\033[94m";
        case Color::BRIGHT_MAGENTA:return "\033[95m";
        case Color::BRIGHT_CYAN:  return "\033[96m";
        case Color::BRIGHT_WHITE: return "\033[97m";
        default: return "";
    }
}

#if CLOG_ENABLE_TAG_FILTERING
// Tag filtering implementation
inline bool Logger::checkTagFilter(const char* tag) {
    if (filterMode == TagFilterMode::ALLOW_ALL) {
        return true;
    }
    
    int index = findTagFilter(tag);
    bool tagInFilter = (index >= 0);
    
    if (filterMode == TagFilterMode::WHITELIST) {
        return tagInFilter;  // Only allow tags in the whitelist
    } else { // BLACKLIST
        return !tagInFilter; // Allow all except tags in the blacklist
    }
}

inline int Logger::findTagFilter(const char* tag) {
    for (size_t i = 0; i < CLOG_MAX_TAG_FILTERS; i++) {
        if (tagFilters[i].active && strcmp(tagFilters[i].tag, tag) == 0) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

inline void Logger::enableTag(const char* tag) {
    // If we're in ALLOW_ALL mode, switch to WHITELIST and add this tag
    if (filterMode == TagFilterMode::ALLOW_ALL) {
        filterMode = TagFilterMode::WHITELIST;
    }
    
    // If we're in BLACKLIST mode, remove the tag from blacklist
    if (filterMode == TagFilterMode::BLACKLIST) {
        int index = findTagFilter(tag);
        if (index >= 0) {
            tagFilters[index].active = false;
        }
        return;
    }
    
    // WHITELIST mode: add tag to whitelist if not already present
    if (findTagFilter(tag) >= 0) {
        return; // Tag already enabled
    }
    
    // Find empty slot
    for (size_t i = 0; i < CLOG_MAX_TAG_FILTERS; i++) {
        if (!tagFilters[i].active) {
            strncpy(tagFilters[i].tag, tag, sizeof(tagFilters[i].tag) - 1);
            tagFilters[i].tag[sizeof(tagFilters[i].tag) - 1] = '\0';
            tagFilters[i].active = true;
            return;
        }
    }
}

inline void Logger::disableTag(const char* tag) {
    // If we're in ALLOW_ALL mode, switch to BLACKLIST and add this tag
    if (filterMode == TagFilterMode::ALLOW_ALL) {
        filterMode = TagFilterMode::BLACKLIST;
    }
    
    // If we're in WHITELIST mode, remove the tag from whitelist
    if (filterMode == TagFilterMode::WHITELIST) {
        int index = findTagFilter(tag);
        if (index >= 0) {
            tagFilters[index].active = false;
        }
        return;
    }
    
    // BLACKLIST mode: add tag to blacklist if not already present
    if (findTagFilter(tag) >= 0) {
        return; // Tag already disabled
    }
    
    // Find empty slot
    for (size_t i = 0; i < CLOG_MAX_TAG_FILTERS; i++) {
        if (!tagFilters[i].active) {
            strncpy(tagFilters[i].tag, tag, sizeof(tagFilters[i].tag) - 1);
            tagFilters[i].tag[sizeof(tagFilters[i].tag) - 1] = '\0';
            tagFilters[i].active = true;
            return;
        }
    }
}

inline void Logger::enableAllTags() {
    filterMode = TagFilterMode::ALLOW_ALL;
    clearTagFilters();
}

inline void Logger::disableAllTags() {
    filterMode = TagFilterMode::WHITELIST;
    clearTagFilters();
}

inline bool Logger::isTagEnabled(const char* tag) {
    return checkTagFilter(tag);
}

inline void Logger::clearTagFilters() {
    for (size_t i = 0; i < CLOG_MAX_TAG_FILTERS; i++) {
        tagFilters[i].active = false;
    }
}
#endif // CLOG_ENABLE_TAG_FILTERING

// Library identification and control
inline void Logger::enableLibraryTags(bool enabled) {
    libraryTagsEnabled = enabled;
}

inline bool Logger::isLibraryTagsEnabled() {
    return libraryTagsEnabled;
}

// Library color management
inline void Logger::setLibraryColor(const char* library, Color color) {
    if (!library || strlen(library) == 0) {
        return;  // Handle null or empty library name gracefully
    }
    
    // First check if library already exists
    for (size_t i = 0; i < MAX_LIBRARY_COLORS; i++) {
        if (libraryColors[i].active && strcmp(libraryColors[i].library, library) == 0) {
            libraryColors[i].color = color;
            return;
        }
    }
    
    // Find empty slot
    for (size_t i = 0; i < MAX_LIBRARY_COLORS; i++) {
        if (!libraryColors[i].active) {
            strncpy(libraryColors[i].library, library, sizeof(libraryColors[i].library) - 1);
            libraryColors[i].library[sizeof(libraryColors[i].library) - 1] = '\0';
            libraryColors[i].color = color;
            libraryColors[i].active = true;
            return;
        }
    }
}

inline void Logger::clearLibraryColor(const char* library) {
    if (!library || strlen(library) == 0) {
        return;  // Handle null or empty library name gracefully
    }
    
    for (size_t i = 0; i < MAX_LIBRARY_COLORS; i++) {
        if (libraryColors[i].active && strcmp(libraryColors[i].library, library) == 0) {
            libraryColors[i].active = false;
            return;
        }
    }
}

inline void Logger::clearAllLibraryColors() {
    for (size_t i = 0; i < MAX_LIBRARY_COLORS; i++) {
        libraryColors[i].active = false;
    }
}

inline Color Logger::getLibraryColor(const char* library) {
    for (size_t i = 0; i < MAX_LIBRARY_COLORS; i++) {
        if (libraryColors[i].active && strcmp(libraryColors[i].library, library) == 0) {
            return libraryColors[i].color;
        }
    }
    return Color::DEFAULT;
}

} // namespace clogger