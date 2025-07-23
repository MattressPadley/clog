#include <clog/log.hpp>
#include <clog/platform.hpp>
#include <clog/config.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

// Simple integration test without complex lambda captures
class SimpleIntegrationTest {
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

    static int summary() {
        std::cout << "\n=== Simple Integration Test Summary ===" << std::endl;
        std::cout << "Tests run: " << tests_run << std::endl;
        std::cout << "Tests passed: " << tests_passed << std::endl;
        std::cout << "Tests failed: " << (tests_run - tests_passed) << std::endl;
        return (tests_run == tests_passed) ? 0 : 1;
    }
};

int SimpleIntegrationTest::tests_run = 0;
int SimpleIntegrationTest::tests_passed = 0;

// Global test state for callbacks
std::vector<std::string> test_messages;
int test_message_count = 0;

void simpleTestCallback(clog::Level level, const char* tag, const char* message) {
    test_message_count++;
    const char* level_str = "";
    switch (level) {
        case clog::Level::ERROR: level_str = "ERROR"; break;
        case clog::Level::WARN:  level_str = "WARN"; break;
        case clog::Level::INFO:  level_str = "INFO"; break;
        case clog::Level::DEBUG: level_str = "DEBUG"; break;
        case clog::Level::TRACE: level_str = "TRACE"; break;
        default: level_str = "UNKNOWN"; break;
    }
    
    test_messages.push_back(std::string(level_str) + ":" + tag + ":" + message);
}

void fileTestCallback(clog::Level level, const char* tag, const char* message) {
    std::ofstream file("test_integration.log", std::ios::app);
    if (file.is_open()) {
        file << "[" << tag << "] " << message << std::endl;
    }
}

void test_basic_integration() {
    std::cout << "\n--- Testing Basic Integration ---" << std::endl;
    
    // Clear test state
    test_messages.clear();
    test_message_count = 0;
    
    // Set callback and test
    clog::Logger::setCallback(simpleTestCallback);
    clog::Logger::setLevel(clog::Level::INFO);
    
    CLOG_ERROR("BasicTest", "Error message");
    CLOG_WARN("BasicTest", "Warning message");
    CLOG_INFO("BasicTest", "Info message");
    CLOG_DEBUG("BasicTest", "Debug message");  // Should be filtered
    
    clog::Logger::setCallback(nullptr);
    
    SimpleIntegrationTest::assert_true(test_message_count == 3, "Three messages captured (debug filtered)");
    SimpleIntegrationTest::assert_true(test_messages.size() == 3, "Message vector has 3 entries");
    SimpleIntegrationTest::assert_true(test_messages[0].find("ERROR:BasicTest:Error message") != std::string::npos, "Error message correct");
    SimpleIntegrationTest::assert_true(test_messages[1].find("WARN:BasicTest:Warning message") != std::string::npos, "Warning message correct");
    SimpleIntegrationTest::assert_true(test_messages[2].find("INFO:BasicTest:Info message") != std::string::npos, "Info message correct");
}

void test_file_integration() {
    std::cout << "\n--- Testing File Integration ---" << std::endl;
    
    const std::string log_file = "test_integration.log";
    
    // Remove existing file
    std::remove(log_file.c_str());
    
    // Set file callback
    clog::Logger::setCallback(fileTestCallback);
    clog::Logger::setLevel(clog::Level::INFO);
    
    CLOG_ERROR("FileTest", "File error message");
    CLOG_INFO("FileTest", "File info message");
    CLOG_INFO("FileTest", "Formatted: %d items", 42);
    
    clog::Logger::setCallback(nullptr);
    
    // Check file was created and has content
    std::ifstream file(log_file);
    SimpleIntegrationTest::assert_true(file.is_open(), "Log file created");
    
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    file.close();
    
    SimpleIntegrationTest::assert_true(lines.size() == 3, "Three lines written to file");
    SimpleIntegrationTest::assert_true(lines[0].find("File error message") != std::string::npos, "Error line in file");
    SimpleIntegrationTest::assert_true(lines[2].find("42 items") != std::string::npos, "Formatted line in file");
    
    // Clean up
    std::remove(log_file.c_str());
}

void test_platform_integration() {
    std::cout << "\n--- Testing Platform Integration ---" << std::endl;
    
    // Test platform detection
    const char* platform = clog::platform::getName();
    SimpleIntegrationTest::assert_true(platform != nullptr, "Platform name available");
    SimpleIntegrationTest::assert_true(strlen(platform) > 0, "Platform name not empty");
    
    std::cout << "Detected platform: " << platform << std::endl;
    
    // Test platform features
    bool has_color = clog::platform::hasColorSupport();
    bool is_embedded = clog::platform::isEmbedded();
    size_t buffer_size = clog::platform::getDefaultBufferSize();
    
    SimpleIntegrationTest::assert_true(buffer_size > 0, "Buffer size > 0");
    SimpleIntegrationTest::assert_true(buffer_size >= 64, "Buffer size >= 64");
    
    std::cout << "Color support: " << (has_color ? "Yes" : "No") << std::endl;
    std::cout << "Is embedded: " << (is_embedded ? "Yes" : "No") << std::endl;
    std::cout << "Buffer size: " << buffer_size << std::endl;
    
    // Test platform initialization
    clog::platform::init();  // Should not crash
    SimpleIntegrationTest::assert_true(true, "Platform init completed");
}

void test_direct_output() {
    std::cout << "\n--- Testing Direct Output ---" << std::endl;
    
    // Test without callback (direct output)
    clog::Logger::setCallback(nullptr);
    clog::Logger::setLevel(clog::Level::INFO);
    
    std::cout << "The following should appear as direct output:" << std::endl;
    CLOG_ERROR("DirectTest", "Direct error output");
    CLOG_WARN("DirectTest", "Direct warning output");
    CLOG_INFO("DirectTest", "Direct info output");
    
    SimpleIntegrationTest::assert_true(true, "Direct output test completed without crash");
}

void test_performance_basic() {
    std::cout << "\n--- Testing Basic Performance ---" << std::endl;
    
    test_message_count = 0;
    clog::Logger::setCallback(simpleTestCallback);
    clog::Logger::setLevel(clog::Level::INFO);
    
    // Generate many messages quickly
    const int num_messages = 1000;
    for (int i = 0; i < num_messages; i++) {
        CLOG_INFO("PerfTest", "Message %d", i);
    }
    
    clog::Logger::setCallback(nullptr);
    
    SimpleIntegrationTest::assert_true(test_message_count == num_messages, "All performance messages processed");
    SimpleIntegrationTest::assert_true(test_message_count <= num_messages + 10, "No unexpected extra messages");
    
    std::cout << "Processed " << test_message_count << " messages successfully" << std::endl;
}

void test_edge_cases() {
    std::cout << "\n--- Testing Edge Cases ---" << std::endl;
    
    test_messages.clear();
    test_message_count = 0;
    clog::Logger::setCallback(simpleTestCallback);
    clog::Logger::setLevel(clog::Level::TRACE);
    
    // Test with empty strings
    CLOG_INFO("", "Empty tag test");
    CLOG_INFO("EdgeCase", "");
    
    // Test with special characters
    CLOG_INFO("EdgeCase", "Special chars: %% \\ \" \n \t");
    
    // Test with long tag
    std::string long_tag(50, 'A');
    CLOG_INFO(long_tag.c_str(), "Long tag test");
    
    // Test with long message
    std::string long_message(200, 'B');
    CLOG_INFO("EdgeCase", "%s", long_message.c_str());
    
    clog::Logger::setCallback(nullptr);
    
    SimpleIntegrationTest::assert_true(test_message_count >= 5, "Edge case messages processed");
}

int main() {
    std::cout << "=== CLog Simple Integration Tests ===" << std::endl;
    
    test_basic_integration();
    test_file_integration();
    test_platform_integration();
    test_direct_output();
    test_performance_basic();
    test_edge_cases();
    
    return SimpleIntegrationTest::summary();
}