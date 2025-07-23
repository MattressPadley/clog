#pragma once

// CLog Library Configuration
// This header defines compile-time configuration options for the CLog library

// Default log level (can be overridden by defining CLOG_LEVEL before including)
#ifndef CLOG_LEVEL
    #ifdef DEBUG
        #define CLOG_LEVEL 4  // DEBUG level for debug builds
    #else
        #define CLOG_LEVEL 3  // INFO level for release builds
    #endif
#endif

// Buffer size for log messages (can be overridden by defining CLOG_BUFFER_SIZE)
#ifndef CLOG_BUFFER_SIZE
    #ifdef CLOG_PLATFORM_EMBEDDED
        #define CLOG_BUFFER_SIZE 256  // Smaller buffer for embedded systems
    #else
        #define CLOG_BUFFER_SIZE 512  // Larger buffer for desktop systems
    #endif
#endif

// Maximum tag length (for truncation if needed)
#ifndef CLOG_MAX_TAG_LENGTH
    #define CLOG_MAX_TAG_LENGTH 16
#endif

// Maximum number of tag filters
#ifndef CLOG_MAX_TAG_FILTERS
    #define CLOG_MAX_TAG_FILTERS 16
#endif

// Enable/disable tag filtering feature
#ifndef CLOG_ENABLE_TAG_FILTERING
    #define CLOG_ENABLE_TAG_FILTERING 1
#endif

// Enable/disable features
#ifndef CLOG_ENABLE_COLORS
    #ifdef CLOG_PLATFORM_DESKTOP
        #define CLOG_ENABLE_COLORS 1
    #else
        #define CLOG_ENABLE_COLORS 0
    #endif
#endif

#ifndef CLOG_ENABLE_TIMESTAMPS
    #define CLOG_ENABLE_TIMESTAMPS 0  // Disabled by default to save space
#endif

#ifndef CLOG_ENABLE_FILE_LINE
    #define CLOG_ENABLE_FILE_LINE 0   // Disabled by default to save space
#endif

// Thread safety (currently not implemented but reserved for future use)
#ifndef CLOG_THREAD_SAFE
    #define CLOG_THREAD_SAFE 0
#endif

// Assertion macro configuration
#ifndef CLOG_ENABLE_ASSERTS
    #ifdef DEBUG
        #define CLOG_ENABLE_ASSERTS 1
    #else
        #define CLOG_ENABLE_ASSERTS 0
    #endif
#endif

// Performance optimization flags
#ifndef CLOG_OPTIMIZE_SIZE
    #ifdef CLOG_PLATFORM_EMBEDDED
        #define CLOG_OPTIMIZE_SIZE 1  // Optimize for size on embedded platforms
    #else
        #define CLOG_OPTIMIZE_SIZE 0  // Optimize for performance on desktop
    #endif
#endif

// Configuration validation
namespace clog {
namespace config {

// Validate configuration at compile time
static_assert(CLOG_LEVEL >= 0 && CLOG_LEVEL <= 5, "CLOG_LEVEL must be between 0 and 5");
static_assert(CLOG_BUFFER_SIZE >= 64, "CLOG_BUFFER_SIZE must be at least 64 bytes");
static_assert(CLOG_BUFFER_SIZE <= 4096, "CLOG_BUFFER_SIZE should not exceed 4096 bytes");
static_assert(CLOG_MAX_TAG_LENGTH >= 4, "CLOG_MAX_TAG_LENGTH must be at least 4 characters");
static_assert(CLOG_MAX_TAG_FILTERS >= 1, "CLOG_MAX_TAG_FILTERS must be at least 1");

// Compile-time configuration constants
constexpr int DEFAULT_LEVEL = CLOG_LEVEL;
constexpr size_t BUFFER_SIZE = CLOG_BUFFER_SIZE;
constexpr size_t MAX_TAG_LENGTH = CLOG_MAX_TAG_LENGTH;
constexpr size_t MAX_TAG_FILTERS = CLOG_MAX_TAG_FILTERS;
constexpr bool TAG_FILTERING_ENABLED = CLOG_ENABLE_TAG_FILTERING;
constexpr bool COLORS_ENABLED = CLOG_ENABLE_COLORS;
constexpr bool TIMESTAMPS_ENABLED = CLOG_ENABLE_TIMESTAMPS;
constexpr bool FILE_LINE_ENABLED = CLOG_ENABLE_FILE_LINE;
constexpr bool THREAD_SAFE = CLOG_THREAD_SAFE;
constexpr bool ASSERTS_ENABLED = CLOG_ENABLE_ASSERTS;
constexpr bool SIZE_OPTIMIZED = CLOG_OPTIMIZE_SIZE;

// Configuration summary for debugging
inline void printConfig() {
#ifdef CLOG_PLATFORM_DESKTOP
    printf("CLog Configuration:\n");
    printf("  Default Level: %d\n", DEFAULT_LEVEL);
    printf("  Buffer Size: %zu bytes\n", BUFFER_SIZE);
    printf("  Max Tag Length: %zu chars\n", MAX_TAG_LENGTH);
    printf("  Max Tag Filters: %zu\n", MAX_TAG_FILTERS);
    printf("  Tag Filtering: %s\n", TAG_FILTERING_ENABLED ? "enabled" : "disabled");
    printf("  Colors: %s\n", COLORS_ENABLED ? "enabled" : "disabled");
    printf("  Timestamps: %s\n", TIMESTAMPS_ENABLED ? "enabled" : "disabled");
    printf("  File/Line: %s\n", FILE_LINE_ENABLED ? "enabled" : "disabled");
    printf("  Thread Safe: %s\n", THREAD_SAFE ? "enabled" : "disabled");
    printf("  Asserts: %s\n", ASSERTS_ENABLED ? "enabled" : "disabled");
    printf("  Size Optimized: %s\n", SIZE_OPTIMIZED ? "yes" : "no");
#endif
}

} // namespace config
} // namespace clog

// Enhanced macros with file/line information (if enabled)
#if CLOG_ENABLE_FILE_LINE
    #define CLOG_ERROR_FL(tag, fmt, ...)   clog::Logger::error(tag " (" __FILE__ ":" STRINGIFY(__LINE__) ")", fmt, ##__VA_ARGS__)
    #define CLOG_WARN_FL(tag, fmt, ...)    clog::Logger::warn(tag " (" __FILE__ ":" STRINGIFY(__LINE__) ")", fmt, ##__VA_ARGS__)
    #define CLOG_INFO_FL(tag, fmt, ...)    clog::Logger::info(tag " (" __FILE__ ":" STRINGIFY(__LINE__) ")", fmt, ##__VA_ARGS__)
    #define CLOG_DEBUG_FL(tag, fmt, ...)   clog::Logger::debug(tag " (" __FILE__ ":" STRINGIFY(__LINE__) ")", fmt, ##__VA_ARGS__)
    #define CLOG_TRACE_FL(tag, fmt, ...)   clog::Logger::trace(tag " (" __FILE__ ":" STRINGIFY(__LINE__) ")", fmt, ##__VA_ARGS__)
    
    #define STRINGIFY(x) #x
#endif

// Assert macro (if enabled)
#if CLOG_ENABLE_ASSERTS
    #define CLOG_ASSERT(condition, tag, fmt, ...) \
        do { \
            if (!(condition)) { \
                CLOG_ERROR(tag, "ASSERTION FAILED: " #condition " - " fmt, ##__VA_ARGS__); \
            } \
        } while(0)
#else
    #define CLOG_ASSERT(condition, tag, fmt, ...) ((void)0)
#endif