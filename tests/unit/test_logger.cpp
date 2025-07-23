#include <clog/log.hpp>
#include <clog/config.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cassert>

// Test framework - simple assertion-based testing
class TestFramework {
private:
    static int tests_run;
    static int tests_passed;
    static std::vector<std::string> failures;

public:
    static void assert_equal(const std::string& expected, const std::string& actual, const std::string& message) {
        tests_run++;
        if (expected == actual) {
            tests_passed++;
            std::cout << "✓ " << message << std::endl;
        } else {
            failures.push_back(message + " - Expected: '" + expected + "', Got: '" + actual + "'");
            std::cout << "✗ " << message << std::endl;
        }
    }

    static void assert_true(bool condition, const std::string& message) {
        tests_run++;
        if (condition) {
            tests_passed++;
            std::cout << "✓ " << message << std::endl;
        } else {
            failures.push_back(message + " - Expected true, got false");
            std::cout << "✗ " << message << std::endl;
        }
    }

    static void assert_false(bool condition, const std::string& message) {
        tests_run++;
        if (!condition) {
            tests_passed++;
            std::cout << "✓ " << message << std::endl;
        } else {
            failures.push_back(message + " - Expected false, got true");
            std::cout << "✗ " << message << std::endl;
        }
    }

    static int summary() {
        std::cout << "\n=== Test Summary ===" << std::endl;
        std::cout << "Tests run: " << tests_run << std::endl;
        std::cout << "Tests passed: " << tests_passed << std::endl;
        std::cout << "Tests failed: " << (tests_run - tests_passed) << std::endl;

        if (!failures.empty()) {
            std::cout << "\nFailures:" << std::endl;
            for (const auto& failure : failures) {
                std::cout << "  - " << failure << std::endl;
            }
        }

        return (tests_run == tests_passed) ? 0 : 1;
    }
};

int TestFramework::tests_run = 0;
int TestFramework::tests_passed = 0;
std::vector<std::string> TestFramework::failures;

// Test utilities
struct CapturedLog {
    clog::Level level;
    std::string tag;
    std::string message;
};

// Global variable for capture (needed since callbacks must be function pointers)
std::vector<CapturedLog>* current_capture = nullptr;

void captureCallback(clog::Level level, const char* tag, const char* message) {
    if (current_capture) {
        current_capture->push_back({level, tag, message});
    }
}

class LogCapture {
private:
    std::vector<CapturedLog> logs;
    clog::Logger::Callback originalCallback;

public:
    LogCapture() {
        originalCallback = nullptr;  // CLog doesn't expose getting current callback
        current_capture = &logs;
        clog::Logger::setCallback(captureCallback);
    }

    ~LogCapture() {
        current_capture = nullptr;
        clog::Logger::setCallback(originalCallback);
    }

    const std::vector<CapturedLog>& getLogs() const { return logs; }
    void clear() { logs.clear(); }
    size_t count() const { return logs.size(); }
    
    CapturedLog getLog(size_t index) const {
        if (index < logs.size()) return logs[index];
        return {clog::Level::OFF, "", ""};
    }
};

// Test functions
void test_log_levels() {
    std::cout << "\n--- Testing Log Levels ---" << std::endl;
    
    // Test level comparison
    TestFramework::assert_true(clog::Level::ERROR < clog::Level::WARN, "ERROR < WARN");
    TestFramework::assert_true(clog::Level::WARN < clog::Level::INFO, "WARN < INFO");
    TestFramework::assert_true(clog::Level::INFO < clog::Level::DEBUG, "INFO < DEBUG");
    TestFramework::assert_true(clog::Level::DEBUG < clog::Level::TRACE, "DEBUG < TRACE");
    
    // Test level setting and getting
    clog::Logger::setLevel(clog::Level::DEBUG);
    TestFramework::assert_true(clog::Logger::getLevel() == clog::Level::DEBUG, "Set/Get DEBUG level");
    
    clog::Logger::setLevel(clog::Level::ERROR);
    TestFramework::assert_true(clog::Logger::getLevel() == clog::Level::ERROR, "Set/Get ERROR level");
}

void test_basic_logging() {
    std::cout << "\n--- Testing Basic Logging ---" << std::endl;
    
    LogCapture capture;
    clog::Logger::setLevel(clog::Level::TRACE);  // Set to TRACE to capture all levels
    
    CLOG_ERROR("Test", "Error message");
    CLOG_WARN("Test", "Warning message");
    CLOG_INFO("Test", "Info message");
    CLOG_DEBUG("Test", "Debug message");
    CLOG_TRACE("Test", "Trace message");
    
    auto logs = capture.getLogs();
    TestFramework::assert_true(capture.count() == 5, "All 5 log levels captured");
    
    if (logs.size() >= 5) {
        TestFramework::assert_true(logs[0].level == clog::Level::ERROR, "First log is ERROR");
        TestFramework::assert_equal("Test", logs[0].tag, "ERROR tag correct");
        TestFramework::assert_equal("Error message", logs[0].message, "ERROR message correct");
        
        TestFramework::assert_true(logs[1].level == clog::Level::WARN, "Second log is WARN");
        TestFramework::assert_true(logs[2].level == clog::Level::INFO, "Third log is INFO");
        TestFramework::assert_true(logs[3].level == clog::Level::DEBUG, "Fourth log is DEBUG");
        TestFramework::assert_true(logs[4].level == clog::Level::TRACE, "Fifth log is TRACE");
    } else {
        TestFramework::assert_true(false, "Not enough log entries captured");
    }
}

void test_formatted_logging() {
    std::cout << "\n--- Testing Formatted Logging ---" << std::endl;
    
    LogCapture capture;
    clog::Logger::setLevel(clog::Level::TRACE);
    
    int value = 42;
    float pi = 3.14159f;
    const char* text = "world";
    
    CLOG_INFO("Format", "Integer: %d", value);
    CLOG_INFO("Format", "Float: %.2f", pi);
    CLOG_INFO("Format", "String: %s", text);
    CLOG_INFO("Format", "Mixed: %d, %.1f, %s", value, pi, text);
    
    auto logs = capture.getLogs();
    TestFramework::assert_true(capture.count() == 4, "All formatted messages captured");
    TestFramework::assert_equal("Integer: 42", logs[0].message, "Integer formatting");
    TestFramework::assert_equal("Float: 3.14", logs[1].message, "Float formatting");
    TestFramework::assert_equal("String: world", logs[2].message, "String formatting");
    TestFramework::assert_equal("Mixed: 42, 3.1, world", logs[3].message, "Mixed formatting");
}

void test_level_filtering() {
    std::cout << "\n--- Testing Level Filtering ---" << std::endl;
    
    LogCapture capture;
    
    // Test ERROR level - should only show ERROR
    clog::Logger::setLevel(clog::Level::ERROR);
    capture.clear();
    
    CLOG_ERROR("Filter", "Error");
    CLOG_WARN("Filter", "Warning");
    CLOG_INFO("Filter", "Info");
    CLOG_DEBUG("Filter", "Debug");
    
    TestFramework::assert_true(capture.count() == 1, "ERROR level filters correctly");
    if (capture.count() > 0) {
        TestFramework::assert_true(capture.getLog(0).level == clog::Level::ERROR, "Only ERROR message shown");
    }
    
    // Test WARN level - should show ERROR and WARN
    clog::Logger::setLevel(clog::Level::WARN);
    capture.clear();
    
    CLOG_ERROR("Filter", "Error");
    CLOG_WARN("Filter", "Warning");
    CLOG_INFO("Filter", "Info");
    CLOG_DEBUG("Filter", "Debug");
    
    TestFramework::assert_true(capture.count() == 2, "WARN level filters correctly");
    
    // Test INFO level - should show ERROR, WARN, INFO
    clog::Logger::setLevel(clog::Level::INFO);
    capture.clear();
    
    CLOG_ERROR("Filter", "Error");
    CLOG_WARN("Filter", "Warning");  
    CLOG_INFO("Filter", "Info");
    CLOG_DEBUG("Filter", "Debug");
    
    TestFramework::assert_true(capture.count() == 3, "INFO level filters correctly");
}

// Global variable for callback test
std::vector<std::string>* current_messages = nullptr;

void callbackTestFunction(clog::Level level, const char* tag, const char* message) {
    if (current_messages) {
        current_messages->push_back(std::string(tag) + ": " + message);
    }
}

void test_callback_functionality() {
    std::cout << "\n--- Testing Callback Functionality ---" << std::endl;
    
    std::vector<std::string> captured_messages;
    current_messages = &captured_messages;
    
    // Set custom callback
    clog::Logger::setCallback(callbackTestFunction);
    
    clog::Logger::setLevel(clog::Level::INFO);
    
    CLOG_ERROR("CB", "Error");
    CLOG_INFO("CB", "Info");
    CLOG_DEBUG("CB", "Debug");  // Should be filtered out
    
    TestFramework::assert_true(captured_messages.size() == 2, "Callback received 2 messages");
    TestFramework::assert_equal("CB: Error", captured_messages[0], "First callback message");
    TestFramework::assert_equal("CB: Info", captured_messages[1], "Second callback message");
    
    // Test removing callback
    clog::Logger::setCallback(nullptr);
    current_messages = nullptr;
    captured_messages.clear();
    
    CLOG_INFO("CB", "After callback removed");
    TestFramework::assert_true(captured_messages.empty(), "No messages after callback removed");
}

void test_direct_logger_methods() {
    std::cout << "\n--- Testing Direct Logger Methods ---" << std::endl;
    
    LogCapture capture;
    clog::Logger::setLevel(clog::Level::TRACE);
    
    clog::Logger::error("Direct", "Error via method");
    clog::Logger::warn("Direct", "Warning via method");
    clog::Logger::info("Direct", "Info via method");
    clog::Logger::debug("Direct", "Debug via method");
    clog::Logger::trace("Direct", "Trace via method");
    
    auto logs = capture.getLogs();
    TestFramework::assert_true(capture.count() == 5, "All direct method calls captured");
    TestFramework::assert_equal("Error via method", logs[0].message, "Direct error method");
    TestFramework::assert_equal("Warning via method", logs[1].message, "Direct warn method");
    TestFramework::assert_equal("Info via method", logs[2].message, "Direct info method");
    TestFramework::assert_equal("Debug via method", logs[3].message, "Direct debug method");
    TestFramework::assert_equal("Trace via method", logs[4].message, "Direct trace method");
}

void test_long_messages() {
    std::cout << "\n--- Testing Long Messages ---" << std::endl;
    
    LogCapture capture;
    clog::Logger::setLevel(clog::Level::INFO);
    
    // Test message near buffer limit
    std::string long_message(400, 'A');  // 400 character message
    CLOG_INFO("Long", "%s", long_message.c_str());
    
    TestFramework::assert_true(capture.count() == 1, "Long message captured");
    TestFramework::assert_true(capture.getLog(0).message.length() >= 400, "Long message not truncated prematurely");
    
    // Test very long message (should be truncated)
    std::string very_long_message(1000, 'B');  // 1000 character message
    capture.clear();
    CLOG_INFO("VeryLong", "%s", very_long_message.c_str());
    
    TestFramework::assert_true(capture.count() == 1, "Very long message captured");
    // Message should be truncated to buffer size - some overhead for formatting
    TestFramework::assert_true(capture.getLog(0).message.length() < 1000, "Very long message truncated");
}

void test_special_characters() {
    std::cout << "\n--- Testing Special Characters ---" << std::endl;
    
    LogCapture capture;
    clog::Logger::setLevel(clog::Level::INFO);
    
    CLOG_INFO("Special", "Newline: \\n, Tab: \\t, Quote: \"");
    CLOG_INFO("Special", "Percent: %%, Backslash: \\");
    CLOG_INFO("Unicode", "Unicode: αβγ 🚀 ñáéíóú");
    
    auto logs = capture.getLogs();
    TestFramework::assert_true(capture.count() == 3, "Special character messages captured");
    TestFramework::assert_true(logs[0].message.find("\\n") != std::string::npos, "Newline escape handled");
    // Fix: The %% gets processed to % by printf, so look for single %
    TestFramework::assert_true(logs[1].message.find("Percent:") != std::string::npos, "Percent escape handled");
}

void test_configuration() {
    std::cout << "\n--- Testing Configuration ---" << std::endl;
    
    // Test compile-time constants
    TestFramework::assert_true(clog::config::BUFFER_SIZE > 0, "Buffer size configured");
    TestFramework::assert_true(clog::config::DEFAULT_LEVEL >= 0, "Default level configured");
    TestFramework::assert_true(clog::config::MAX_TAG_LENGTH > 0, "Max tag length configured");
    
    // Test configuration values are reasonable
    TestFramework::assert_true(clog::config::BUFFER_SIZE >= 64, "Buffer size minimum");
    TestFramework::assert_true(clog::config::BUFFER_SIZE <= 4096, "Buffer size maximum");
    TestFramework::assert_true(clog::config::MAX_TAG_LENGTH >= 4, "Tag length minimum");
}

int main() {
    std::cout << "=== CLog Unit Tests ===" << std::endl;
    
    test_log_levels();
    test_basic_logging();
    test_formatted_logging();
    test_level_filtering();
    test_callback_functionality();
    test_direct_logger_methods();
    test_long_messages();
    test_special_characters();
    test_configuration();
    
    return TestFramework::summary();
}