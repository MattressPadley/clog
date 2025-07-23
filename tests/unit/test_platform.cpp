#include <clog/platform.hpp>
#include <iostream>
#include <string>
#include <cassert>

// Simple test framework
class TestFramework {
private:
    static int tests_run;
    static int tests_passed;

public:
    static void assert_true(bool condition, const std::string& message) {
        tests_run++;
        if (condition) {
            tests_passed++;
            std::cout << "✓ " << message << std::endl;
        } else {
            std::cout << "✗ " << message << std::endl;
        }
    }

    static void assert_false(bool condition, const std::string& message) {
        assert_true(!condition, message);
    }

    static void assert_not_null(const void* ptr, const std::string& message) {
        assert_true(ptr != nullptr, message);
    }

    static void assert_not_empty(const std::string& str, const std::string& message) {
        assert_true(!str.empty(), message);
    }

    static int summary() {
        std::cout << "\n=== Platform Test Summary ===" << std::endl;
        std::cout << "Tests run: " << tests_run << std::endl;
        std::cout << "Tests passed: " << tests_passed << std::endl;
        std::cout << "Tests failed: " << (tests_run - tests_passed) << std::endl;
        return (tests_run == tests_passed) ? 0 : 1;
    }
};

int TestFramework::tests_run = 0;
int TestFramework::tests_passed = 0;

void test_platform_detection() {
    std::cout << "\n--- Testing Platform Detection ---" << std::endl;
    
    // Platform name should not be empty
    const char* platform_name = clog::platform::getName();
    TestFramework::assert_not_null(platform_name, "Platform name not null");
    TestFramework::assert_not_empty(std::string(platform_name), "Platform name not empty");
    
    std::cout << "Detected platform: " << platform_name << std::endl;
    
    // Test that we have some platform defined
    bool platform_detected = false;
    
#ifdef CLOG_PLATFORM_DESKTOP
    platform_detected = true;
    TestFramework::assert_true(true, "Desktop platform detected");
#endif

#ifdef CLOG_PLATFORM_EMBEDDED
    platform_detected = true;
    TestFramework::assert_true(true, "Embedded platform detected");
#endif

#ifdef CLOG_PLATFORM_ARDUINO
    platform_detected = true;
    TestFramework::assert_true(true, "Arduino platform detected");
#endif

#ifdef CLOG_PLATFORM_ESP32
    platform_detected = true;
    TestFramework::assert_true(true, "ESP32 platform detected");
#endif

#ifdef CLOG_PLATFORM_RP2040_SDK
    platform_detected = true;
    TestFramework::assert_true(true, "RP2040 SDK platform detected");
#endif

    TestFramework::assert_true(platform_detected, "At least one platform macro defined");
}

void test_platform_features() {
    std::cout << "\n--- Testing Platform Features ---" << std::endl;
    
    // Test color support detection
    bool has_color = clog::platform::hasColorSupport();
    std::cout << "Color support: " << (has_color ? "Yes" : "No") << std::endl;
    
    // Test printf support detection
    bool has_printf = clog::platform::hasPrintfSupport();
    std::cout << "Printf support: " << (has_printf ? "Yes" : "No") << std::endl;
    
    // Test embedded detection
    bool is_embedded = clog::platform::isEmbedded();
    std::cout << "Is embedded: " << (is_embedded ? "Yes" : "No") << std::endl;
    
    // Test default buffer size
    size_t buffer_size = clog::platform::getDefaultBufferSize();
    TestFramework::assert_true(buffer_size > 0, "Default buffer size > 0");
    TestFramework::assert_true(buffer_size >= 64, "Default buffer size >= 64");
    TestFramework::assert_true(buffer_size <= 2048, "Default buffer size <= 2048");
    
    std::cout << "Default buffer size: " << buffer_size << " bytes" << std::endl;
    
    // Logical consistency checks
#ifdef CLOG_PLATFORM_DESKTOP
    TestFramework::assert_true(has_color, "Desktop should have color support");
    TestFramework::assert_false(is_embedded, "Desktop should not be embedded");
    TestFramework::assert_true(buffer_size >= 512, "Desktop should have larger buffer");
#endif

#ifdef CLOG_PLATFORM_EMBEDDED
    TestFramework::assert_true(is_embedded, "Embedded platform should be detected as embedded");
    TestFramework::assert_true(buffer_size <= 512, "Embedded should have smaller buffer");
#endif
}

void test_platform_initialization() {
    std::cout << "\n--- Testing Platform Initialization ---" << std::endl;
    
    // Init should not crash
    try {
        clog::platform::init();
        TestFramework::assert_true(true, "Platform init completed without exception");
    } catch (...) {
        TestFramework::assert_true(false, "Platform init threw exception");
    }
}

void test_compile_time_constants() {
    std::cout << "\n--- Testing Compile-time Constants ---" << std::endl;
    
    // Test that platform-specific macros have consistent values
    TestFramework::assert_true(CLOG_HAS_COLOR_SUPPORT == 0 || CLOG_HAS_COLOR_SUPPORT == 1, 
                              "Color support is 0 or 1");
    TestFramework::assert_true(CLOG_HAS_PRINTF_SUPPORT == 0 || CLOG_HAS_PRINTF_SUPPORT == 1,
                              "Printf support is 0 or 1");
    
    std::cout << "CLOG_HAS_COLOR_SUPPORT: " << CLOG_HAS_COLOR_SUPPORT << std::endl;
    std::cout << "CLOG_HAS_PRINTF_SUPPORT: " << CLOG_HAS_PRINTF_SUPPORT << std::endl;
    
    // Test consistency between compile-time and runtime values
    TestFramework::assert_true(clog::platform::hasColorSupport() == (CLOG_HAS_COLOR_SUPPORT != 0),
                              "Runtime color support matches compile-time");
    TestFramework::assert_true(clog::platform::hasPrintfSupport() == (CLOG_HAS_PRINTF_SUPPORT != 0),
                              "Runtime printf support matches compile-time");
}

void test_platform_specific_behavior() {
    std::cout << "\n--- Testing Platform-Specific Behavior ---" << std::endl;
    
    const char* platform = clog::platform::getName();
    
    // Test platform-specific expectations
    if (std::string(platform) == "Linux" || std::string(platform) == "macOS" || std::string(platform) == "Windows") {
        TestFramework::assert_true(clog::platform::hasColorSupport(), "Desktop platforms should support colors");
        TestFramework::assert_false(clog::platform::isEmbedded(), "Desktop platforms are not embedded");
    }
    
    if (std::string(platform) == "ESP32" || std::string(platform) == "RP2040-SDK" || std::string(platform) == "Arduino-AVR") {
        TestFramework::assert_true(clog::platform::isEmbedded(), "Embedded platforms should be detected as embedded");
        TestFramework::assert_true(clog::platform::getDefaultBufferSize() <= 512, "Embedded platforms should have smaller buffers");
    }
    
    // All platforms should have a reasonable buffer size
    size_t buffer_size = clog::platform::getDefaultBufferSize();
    TestFramework::assert_true(buffer_size >= 64 && buffer_size <= 2048, "Buffer size in reasonable range");
}

int main() {
    std::cout << "=== CLog Platform Tests ===" << std::endl;
    
    test_platform_detection();
    test_platform_features();
    test_platform_initialization();
    test_compile_time_constants();
    test_platform_specific_behavior();
    
    return TestFramework::summary();
}