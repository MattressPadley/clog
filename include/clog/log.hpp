#pragma once
#include <cstdarg>
#include <cstdio>

// Platform detection
#if defined(ARDUINO) || defined(ESP32) || defined(ESP_PLATFORM)
    #define CLOG_PLATFORM_EMBEDDED
    #ifdef ARDUINO
        #include <Arduino.h>
        #define CLOG_PLATFORM_ARDUINO
    #endif
#elif defined(_WIN32) || defined(__linux__) || defined(__APPLE__)
    #define CLOG_PLATFORM_DESKTOP
    #include <iostream>
#else
    #define CLOG_PLATFORM_UNKNOWN
#endif

namespace clog {

enum class Level {
    OFF = 0,
    ERROR = 1,
    WARN = 2, 
    INFO = 3,
    DEBUG = 4,
    TRACE = 5
};

class Logger {
public:
    // Core API
    static void log(Level level, const char* tag, const char* format, ...);
    static void setLevel(Level level);
    static Level getLevel();
    
    // Callback integration for parent applications
    using Callback = void(*)(Level level, const char* tag, const char* message);
    static void setCallback(Callback callback);
    static void enableDirectOutput(bool enabled = true);
    
    // Platform-specific initialization (optional)
    static void init();
    
    // Convenience methods
    static void error(const char* tag, const char* format, ...);
    static void warn(const char* tag, const char* format, ...);
    static void info(const char* tag, const char* format, ...);
    static void debug(const char* tag, const char* format, ...);
    static void trace(const char* tag, const char* format, ...);
    
private:
    static Level currentLevel;
    static Callback logCallback;
    static bool directOutput;
    static void output(Level level, const char* tag, const char* message);
    static const char* levelToString(Level level);
    static const char* levelToColor(Level level);
};

} // namespace clog

// Convenience macros for easy usage
#define CLOG_ERROR(tag, fmt, ...)   clog::Logger::error(tag, fmt, ##__VA_ARGS__)
#define CLOG_WARN(tag, fmt, ...)    clog::Logger::warn(tag, fmt, ##__VA_ARGS__)
#define CLOG_INFO(tag, fmt, ...)    clog::Logger::info(tag, fmt, ##__VA_ARGS__)
#define CLOG_DEBUG(tag, fmt, ...)   clog::Logger::debug(tag, fmt, ##__VA_ARGS__)
#define CLOG_TRACE(tag, fmt, ...)   clog::Logger::trace(tag, fmt, ##__VA_ARGS__)

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
namespace clog {

// Static member definitions
Level Logger::currentLevel = Level::INFO;
Logger::Callback Logger::logCallback = nullptr;
bool Logger::directOutput = true;

void Logger::log(Level level, const char* tag, const char* format, ...) {
    if (level > currentLevel) return;
    
    char buffer[512];  // Fixed size for embedded compatibility
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    output(level, tag, buffer);
}

void Logger::output(Level level, const char* tag, const char* message) {
    if (logCallback) {
        logCallback(level, tag, message);
    } else if (directOutput) {
        const char* levelStr = levelToString(level);
        
#ifdef CLOG_PLATFORM_ARDUINO
        Serial.printf("[%s] %s: %s\n", levelStr, tag, message);
#elif defined(CLOG_PLATFORM_DESKTOP)
        const char* color = levelToColor(level);
        std::cout << color << "[" << levelStr << "] " << tag << ": " 
                  << message << "\033[0m" << std::endl;
#else
        printf("[%s] %s: %s\n", levelStr, tag, message);
#endif
    }
}

void Logger::setCallback(Callback callback) {
    logCallback = callback;
    directOutput = (callback == nullptr);
}

const char* Logger::levelToString(Level level) {
    switch (level) {
        case Level::ERROR: return "ERROR";
        case Level::WARN:  return "WARN ";
        case Level::INFO:  return "INFO ";
        case Level::DEBUG: return "DEBUG";
        case Level::TRACE: return "TRACE";
        default: return "?????";
    }
}

const char* Logger::levelToColor(Level level) {
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
void Logger::error(const char* tag, const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[512];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    output(Level::ERROR, tag, buffer);
}

void Logger::warn(const char* tag, const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[512];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    output(Level::WARN, tag, buffer);
}

void Logger::info(const char* tag, const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[512];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    output(Level::INFO, tag, buffer);
}

void Logger::debug(const char* tag, const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[512];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    output(Level::DEBUG, tag, buffer);
}

void Logger::trace(const char* tag, const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[512];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    output(Level::TRACE, tag, buffer);
}

void Logger::setLevel(Level level) { currentLevel = level; }
Level Logger::getLevel() { return currentLevel; }
void Logger::enableDirectOutput(bool enabled) { directOutput = enabled; }
void Logger::init() {
    // Platform-specific initialization if needed
#ifdef CLOG_PLATFORM_ARDUINO
    // Serial already initialized by Arduino framework
#endif
}

} // namespace clog