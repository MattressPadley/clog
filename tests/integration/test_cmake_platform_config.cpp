#include <iostream>
#include <string>
#include <cstdlib>
#include "../../include/clog/log.hpp"

// Test suite for CMake platform configuration
class CMakePlatformConfigTest {
private:
    int tests_run = 0;
    int tests_passed = 0;
    
    void assert_true(bool condition, const std::string& test_name) {
        tests_run++;
        if (condition) {
            tests_passed++;
            std::cout << "✓ " << test_name << std::endl;
        } else {
            std::cout << "✗ " << test_name << std::endl;
        }
    }
    
public:
    void run_all_tests() {
        std::cout << "=== CLog CMake Platform Configuration Tests ===" << std::endl;
        std::cout << std::endl;
        
        test_cmake_platform_detection();
        test_auto_detect_fallback();
        test_platform_feature_consistency();
        test_cmake_vs_runtime_priority();
        
        std::cout << std::endl;
        std::cout << "=== CMake Platform Config Test Summary ===" << std::endl;
        std::cout << "Tests run: " << tests_run << std::endl;
        std::cout << "Tests passed: " << tests_passed << std::endl;
        std::cout << "Tests failed: " << (tests_run - tests_passed) << std::endl;
        
        if (tests_passed == tests_run) {
            std::cout << "✅ All CMake platform configuration tests passed!" << std::endl;
        } else {
            std::cout << "❌ Some tests failed!" << std::endl;
            std::exit(1);
        }
    }
    
private:
    void test_cmake_platform_detection() {
        std::cout << "--- Testing CMake Platform Detection ---" << std::endl;
        
        // Test that CMake platform configuration takes priority
        #if defined(CLOG_PLATFORM_DESKTOP)
            assert_true(clogger::Logger::isDesktopPlatform(), "CMake DESKTOP platform detected");
            assert_true(!clogger::Logger::isEmbeddedPlatform(), "CMake DESKTOP not embedded");
            assert_true(!clogger::Logger::isArduinoPlatform(), "CMake DESKTOP not Arduino");
            std::cout << "Configured platform: DESKTOP (via CMake)" << std::endl;
        #elif defined(CLOG_PLATFORM_WINDOWS)
            assert_true(clogger::Logger::isDesktopPlatform(), "CMake WINDOWS platform detected");
            assert_true(!clogger::Logger::isEmbeddedPlatform(), "CMake WINDOWS not embedded");
            std::cout << "Configured platform: WINDOWS (via CMake)" << std::endl;
        #elif defined(CLOG_PLATFORM_LINUX)
            assert_true(clogger::Logger::isDesktopPlatform(), "CMake LINUX platform detected");
            assert_true(!clogger::Logger::isEmbeddedPlatform(), "CMake LINUX not embedded");
            std::cout << "Configured platform: LINUX (via CMake)" << std::endl;
        #elif defined(CLOG_PLATFORM_MACOS)
            assert_true(clogger::Logger::isDesktopPlatform(), "CMake MACOS platform detected");
            assert_true(!clogger::Logger::isEmbeddedPlatform(), "CMake MACOS not embedded");
            std::cout << "Configured platform: MACOS (via CMake)" << std::endl;
        #elif defined(CLOG_PLATFORM_ARDUINO)
            assert_true(clogger::Logger::isArduinoPlatform(), "CMake ARDUINO platform detected");
            assert_true(clogger::Logger::isEmbeddedPlatform(), "CMake ARDUINO is embedded");
            assert_true(!clogger::Logger::isDesktopPlatform(), "CMake ARDUINO not desktop");
            std::cout << "Configured platform: ARDUINO (via CMake)" << std::endl;
        #elif defined(CLOG_PLATFORM_ESP32)
            assert_true(clogger::Logger::isArduinoPlatform(), "CMake ESP32 platform detected");
            assert_true(clogger::Logger::isEmbeddedPlatform(), "CMake ESP32 is embedded");
            std::cout << "Configured platform: ESP32 (via CMake)" << std::endl;
        #elif defined(CLOG_PLATFORM_ESP8266)
            assert_true(clogger::Logger::isArduinoPlatform(), "CMake ESP8266 platform detected");
            assert_true(clogger::Logger::isEmbeddedPlatform(), "CMake ESP8266 is embedded");
            std::cout << "Configured platform: ESP8266 (via CMake)" << std::endl;
        #elif defined(CLOG_PLATFORM_RP2040_ARDUINO)
            assert_true(clogger::Logger::isArduinoPlatform(), "CMake RP2040_ARDUINO platform detected");
            assert_true(clogger::Logger::isEmbeddedPlatform(), "CMake RP2040_ARDUINO is embedded");
            std::cout << "Configured platform: RP2040_ARDUINO (via CMake)" << std::endl;
        #elif defined(CLOG_PLATFORM_RP2040_SDK)
            assert_true(!clogger::Logger::isArduinoPlatform(), "CMake RP2040_SDK not Arduino");
            assert_true(clogger::Logger::isEmbeddedPlatform(), "CMake RP2040_SDK is embedded");
            assert_true(!clogger::Logger::isDesktopPlatform(), "CMake RP2040_SDK not desktop");
            std::cout << "Configured platform: RP2040_SDK (via CMake)" << std::endl;
        #elif defined(CLOG_PLATFORM_ESP_IDF)
            assert_true(!clogger::Logger::isArduinoPlatform(), "CMake ESP_IDF not Arduino");
            assert_true(clogger::Logger::isEmbeddedPlatform(), "CMake ESP_IDF is embedded");
            assert_true(!clogger::Logger::isDesktopPlatform(), "CMake ESP_IDF not desktop");
            std::cout << "Configured platform: ESP_IDF (via CMake)" << std::endl;
        #else
            std::cout << "No CMake platform configuration detected - using AUTO_DETECT" << std::endl;
            // This is expected when CLOG_PLATFORM=AUTO_DETECT
        #endif
    }
    
    void test_auto_detect_fallback() {
        std::cout << std::endl;
        std::cout << "--- Testing AUTO_DETECT Fallback Logic ---" << std::endl;
        
        // Test AUTO_DETECT fallback behavior
        clogger::Platform original_platform = clogger::Logger::getPlatform();
        clogger::Logger::setPlatform(clogger::Platform::AUTO_DETECT);
        
        // Test that AUTO_DETECT properly falls back to compile-time detection
        bool desktop_detected = clogger::Logger::isDesktopPlatform();
        bool embedded_detected = clogger::Logger::isEmbeddedPlatform();
        bool arduino_detected = clogger::Logger::isArduinoPlatform();
        
        std::cout << "AUTO_DETECT results:" << std::endl;
        std::cout << "  Desktop: " << (desktop_detected ? "Yes" : "No") << std::endl;
        std::cout << "  Embedded: " << (embedded_detected ? "Yes" : "No") << std::endl;
        std::cout << "  Arduino: " << (arduino_detected ? "Yes" : "No") << std::endl;
        
        // At least one should be true, and they should be mutually exclusive
        assert_true(desktop_detected || embedded_detected, "AUTO_DETECT detects some platform");
        assert_true(!(desktop_detected && embedded_detected), "Desktop and embedded are mutually exclusive");
        
        // Test color and printf support with AUTO_DETECT
        bool color_support = clogger::Logger::hasColorSupport();
        bool printf_support = clogger::Logger::hasPrintfSupport();
        
        std::cout << "  Color support: " << (color_support ? "Yes" : "No") << std::endl;
        std::cout << "  Printf support: " << (printf_support ? "Yes" : "No") << std::endl;
        
        assert_true(true, "AUTO_DETECT fallback completed without crash");
        
        // Restore original platform
        clogger::Logger::setPlatform(original_platform);
    }
    
    void test_platform_feature_consistency() {
        std::cout << std::endl;
        std::cout << "--- Testing Platform Feature Consistency ---" << std::endl;
        
        bool is_desktop = clogger::Logger::isDesktopPlatform();
        bool is_embedded = clogger::Logger::isEmbeddedPlatform();
        bool is_arduino = clogger::Logger::isArduinoPlatform();
        bool has_colors = clogger::Logger::hasColorSupport();
        bool has_printf = clogger::Logger::hasPrintfSupport();
        
        // Consistency checks
        assert_true(!(is_desktop && is_embedded), "Desktop and embedded are mutually exclusive");
        
        if (is_desktop) {
            // Desktop platforms should have color support
            assert_true(has_colors, "Desktop platforms should have color support");
            std::cout << "Desktop platform features validated" << std::endl;
        }
        
        if (is_arduino) {
            // Arduino platforms should be embedded and have printf support
            assert_true(is_embedded, "Arduino platforms should be embedded");
            assert_true(has_printf, "Arduino platforms should have printf support");
            std::cout << "Arduino platform features validated" << std::endl;
        }
        
        std::cout << "Platform feature consistency verified" << std::endl;
    }
    
    void test_cmake_vs_runtime_priority() {
        std::cout << std::endl;
        std::cout << "--- Testing CMake vs Runtime Priority ---" << std::endl;
        
        // Store original platform detection results
        bool original_desktop = clogger::Logger::isDesktopPlatform();
        bool original_embedded = clogger::Logger::isEmbeddedPlatform();
        
        // Try to override with runtime configuration
        clogger::Logger::setPlatform(clogger::Platform::ESP32);
        
        // CMake configuration should still take priority
        bool after_runtime_desktop = clogger::Logger::isDesktopPlatform();
        bool after_runtime_embedded = clogger::Logger::isEmbeddedPlatform();
        
        #if defined(CLOG_PLATFORM_DESKTOP) || defined(CLOG_PLATFORM_WINDOWS) || defined(CLOG_PLATFORM_LINUX) || defined(CLOG_PLATFORM_MACOS)
            // If CMake configured for desktop, it should override runtime setting
            assert_true(after_runtime_desktop == original_desktop, "CMake desktop config overrides runtime");
            assert_true(after_runtime_embedded == original_embedded, "CMake embedded config overrides runtime");
            std::cout << "CMake platform configuration takes priority over runtime" << std::endl;
        #elif defined(CLOG_PLATFORM_ARDUINO) || defined(CLOG_PLATFORM_ESP32) || defined(CLOG_PLATFORM_ESP8266) || defined(CLOG_PLATFORM_RP2040_ARDUINO) || defined(CLOG_PLATFORM_RP2040_SDK) || defined(CLOG_PLATFORM_ESP_IDF)
            // If CMake configured for embedded, it should override runtime setting
            assert_true(after_runtime_desktop == original_desktop, "CMake embedded config overrides runtime");
            assert_true(after_runtime_embedded == original_embedded, "CMake embedded config overrides runtime");
            std::cout << "CMake platform configuration takes priority over runtime" << std::endl;
        #else
            // No CMake configuration, runtime should work
            assert_true(after_runtime_embedded, "Runtime ESP32 configuration should work without CMake override");
            std::cout << "Runtime configuration works when no CMake platform configured" << std::endl;
        #endif
        
        // Reset to AUTO_DETECT
        clogger::Logger::setPlatform(clogger::Platform::AUTO_DETECT);
    }
};

int main() {
    CMakePlatformConfigTest test;
    test.run_all_tests();
    return 0;
}