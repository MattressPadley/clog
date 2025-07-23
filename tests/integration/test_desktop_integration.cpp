#include <clog/log.hpp>
#include <clog/platform.hpp>
#include <clog/config.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <mutex>
#include <cstdio>

// Integration test framework
class IntegrationTest {
private:
    static int tests_run;
    static int tests_passed;
    static std::vector<std::string> failures;

public:
    static void assert_true(bool condition, const std::string& message) {
        tests_run++;
        if (condition) {
            tests_passed++;
            std::cout << "✓ " << message << std::endl;
        } else {
            failures.push_back(message);
            std::cout << "✗ " << message << std::endl;
        }
    }

    static void assert_false(bool condition, const std::string& message) {
        assert_true(!condition, message);
    }

    static int summary() {
        std::cout << "\n=== Integration Test Summary ===" << std::endl;
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

int IntegrationTest::tests_run = 0;
int IntegrationTest::tests_passed = 0;
std::vector<std::string> IntegrationTest::failures;

// Test file logging functionality
void test_file_logging_integration() {
    std::cout << "\n--- Testing File Logging Integration ---" << std::endl;
    
    const std::string log_file = "test_output.log";
    std::vector<std::string> captured_logs;
    
    // Set up file logging callback
    clogger::Logger::setCallback([&captured_logs, &log_file](clogger::Level level, const char* tag, const char* message) {
        std::ofstream file(log_file, std::ios::app);
        if (file.is_open()) {
            const char* level_str = "";
            switch (level) {
                case clogger::Level::ERROR: level_str = "ERROR"; break;
                case clogger::Level::WARN:  level_str = "WARN "; break;
                case clogger::Level::INFO:  level_str = "INFO "; break;
                case clogger::Level::DEBUG: level_str = "DEBUG"; break;
                case clogger::Level::TRACE: level_str = "TRACE"; break;
                default: level_str = "?????"; break;
            }
            
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            
            file << "[" << level_str << "] " << tag << ": " << message << std::endl;
            captured_logs.push_back(std::string(tag) + ": " + message);
        }
    });
    
    clogger::Logger::setLevel(clogger::Level::DEBUG);
    
    // Remove any existing log file
    std::remove(log_file.c_str());
    
    // Generate various log messages
    CLOG_ERROR("FileTest", "This is an error message");
    CLOG_WARN("FileTest", "This is a warning message");
    CLOG_INFO("FileTest", "This is an info message");
    CLOG_DEBUG("FileTest", "This is a debug message");
    
    // Test formatted messages
    CLOG_INFO("FileTest", "Formatted message: %d items processed", 42);
    
    // Reset callback
    clogger::Logger::setCallback(nullptr);
    
    // Verify file was created and contains expected content
    std::ifstream file(log_file);
    IntegrationTest::assert_true(file.is_open(), "Log file created successfully");
    
    std::string line;
    std::vector<std::string> file_lines;
    while (std::getline(file, line)) {
        file_lines.push_back(line);
    }
    file.close();
    
    IntegrationTest::assert_true(file_lines.size() == 5, "All log messages written to file");
    IntegrationTest::assert_true(captured_logs.size() == 5, "All log messages captured");
    
    // Verify content
    IntegrationTest::assert_true(file_lines[0].find("ERROR") != std::string::npos, "Error message in file");
    IntegrationTest::assert_true(file_lines[4].find("42 items processed") != std::string::npos, "Formatted message in file");
    
    // Clean up
    std::remove(log_file.c_str());
}

// Test multi-threaded logging
void test_multithreaded_logging() {
    std::cout << "\n--- Testing Multi-threaded Logging ---" << std::endl;
    
    std::vector<std::string> all_messages;
    std::mutex message_mutex;
    
    clogger::Logger::setCallback([&all_messages, &message_mutex](clogger::Level level, const char* tag, const char* message) {
        std::lock_guard<std::mutex> lock(message_mutex);
        all_messages.push_back(std::string(tag) + ": " + message);
    });
    
    clogger::Logger::setLevel(clogger::Level::INFO);
    
    const int num_threads = 4;
    const int messages_per_thread = 10;
    std::vector<std::thread> threads;
    
    // Launch multiple threads that log messages
    for (int t = 0; t < num_threads; t++) {
        threads.emplace_back([t, messages_per_thread]() {
            for (int i = 0; i < messages_per_thread; i++) {
                CLOG_INFO("Thread" + std::to_string(t), "Message %d from thread %d", i, t);
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    clogger::Logger::setCallback(nullptr);
    
    // Verify all messages were captured
    IntegrationTest::assert_true(all_messages.size() == num_threads * messages_per_thread, 
                                "All threaded messages captured");
    
    // Verify messages from each thread are present
    for (int t = 0; t < num_threads; t++) {
        std::string thread_tag = "Thread" + std::to_string(t);
        int thread_message_count = 0;
        for (const auto& msg : all_messages) {
            if (msg.find(thread_tag) != std::string::npos) {
                thread_message_count++;
            }
        }
        IntegrationTest::assert_true(thread_message_count == messages_per_thread, 
                                    "Thread " + std::to_string(t) + " messages captured");
    }
}

// Test performance under load
void test_performance_integration() {
    std::cout << "\n--- Testing Performance Integration ---" << std::endl;
    
    int message_count = 0;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    clogger::Logger::setCallback([&message_count](clogger::Level level, const char* tag, const char* message) {
        message_count++;
        // Simulate some processing overhead
        volatile int dummy = 0;
        for (int i = 0; i < 10; i++) dummy += i;
    });
    
    clogger::Logger::setLevel(clogger::Level::INFO);
    
    const int num_messages = 1000;
    
    // Generate many log messages
    for (int i = 0; i < num_messages; i++) {
        CLOG_INFO("PerfTest", "Performance test message %d", i);
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    clogger::Logger::setCallback(nullptr);
    
    IntegrationTest::assert_true(message_count == num_messages, "All performance test messages processed");
    IntegrationTest::assert_true(duration.count() < 5000, "Performance test completed in reasonable time");  // < 5 seconds
    
    std::cout << "Generated " << num_messages << " messages in " << duration.count() << "ms" << std::endl;
    std::cout << "Rate: " << (num_messages * 1000.0 / duration.count()) << " messages/second" << std::endl;
}

// Test integration with different platforms
void test_platform_integration() {
    std::cout << "\n--- Testing Platform Integration ---" << std::endl;
    
    // Initialize platform
    clogger::platform::init();
    
    // Test that platform-specific features work
    std::cout << "Platform: " << clogger::platform::getName() << std::endl;
    std::cout << "Buffer size: " << clogger::platform::getDefaultBufferSize() << std::endl;
    std::cout << "Color support: " << (clogger::platform::hasColorSupport() ? "Yes" : "No") << std::endl;
    std::cout << "Printf support: " << (clogger::platform::hasPrintfSupport() ? "Yes" : "No") << std::endl;
    
    // Test direct output (without callback)
    clogger::Logger::setCallback(nullptr);
    clogger::Logger::setLevel(clogger::Level::INFO);
    
    std::cout << "\nDirect output test (should appear in console):" << std::endl;
    CLOG_ERROR("Platform", "Platform integration error test");
    CLOG_WARN("Platform", "Platform integration warning test");
    CLOG_INFO("Platform", "Platform integration info test");
    
    IntegrationTest::assert_true(true, "Platform integration completed without crashes");
}

// Test configuration integration
void test_configuration_integration() {
    std::cout << "\n--- Testing Configuration Integration ---" << std::endl;
    
    // Test that configuration values are accessible and reasonable
    std::cout << "Buffer size: " << clogger::config::BUFFER_SIZE << std::endl;
    std::cout << "Default level: " << static_cast<int>(clogger::config::DEFAULT_LEVEL) << std::endl;
    std::cout << "Max tag length: " << clogger::config::MAX_TAG_LENGTH << std::endl;
    
    IntegrationTest::assert_true(clogger::config::BUFFER_SIZE >= 64, "Buffer size minimum");
    IntegrationTest::assert_true(clogger::config::BUFFER_SIZE <= 4096, "Buffer size maximum");
    IntegrationTest::assert_true(static_cast<int>(clogger::config::DEFAULT_LEVEL) >= 0, "Default level minimum");
    IntegrationTest::assert_true(static_cast<int>(clogger::config::DEFAULT_LEVEL) <= 5, "Default level maximum");
    
    // Test very long tag names
    std::string long_tag(100, 'A');  // 100 character tag
    
    std::vector<std::string> captured;
    clogger::Logger::setCallback([&captured](clogger::Level level, const char* tag, const char* message) {
        captured.push_back(std::string(tag) + ": " + message);
    });
    
    clogger::Logger::setLevel(clogger::Level::INFO);
    CLOG_INFO(long_tag.c_str(), "Test with very long tag");
    
    clogger::Logger::setCallback(nullptr);
    
    IntegrationTest::assert_true(captured.size() == 1, "Long tag message captured");
    IntegrationTest::assert_true(captured[0].find("Test with very long tag") != std::string::npos, "Long tag message content correct");
}

// Test real-world usage scenario
void test_real_world_scenario() {
    std::cout << "\n--- Testing Real-world Scenario ---" << std::endl;
    
    struct LogStats {
        int errors = 0;
        int warnings = 0; 
        int infos = 0;
        int debugs = 0;
    };
    
    LogStats stats;
    
    clogger::Logger::setCallback([&stats](clogger::Level level, const char* tag, const char* message) {
        switch (level) {
            case clogger::Level::ERROR: stats.errors++; break;
            case clogger::Level::WARN: stats.warnings++; break;
            case clogger::Level::INFO: stats.infos++; break;
            case clogger::Level::DEBUG: stats.debugs++; break;
            default: break;
        }
    });
    
    clogger::Logger::setLevel(clogger::Level::DEBUG);
    
    // Simulate a real application workflow
    CLOG_INFO("App", "Application starting...");
    
    // Simulate initialization phase
    for (int i = 0; i < 5; i++) {
        CLOG_DEBUG("Init", "Initializing component %d", i);
        if (i == 2) {
            CLOG_WARN("Init", "Component %d took longer than expected", i);
        }
    }
    
    // Simulate main processing loop
    for (int i = 0; i < 10; i++) {
        CLOG_DEBUG("Processing", "Processing item %d", i);
        
        if (i % 3 == 0 && i > 0) {
            CLOG_INFO("Processing", "Checkpoint: processed %d items", i);
        }
        
        if (i == 7) {
            CLOG_ERROR("Processing", "Failed to process item %d", i);
        }
    }
    
    CLOG_INFO("App", "Application shutdown complete");
    
    clogger::Logger::setCallback(nullptr);
    
    // Verify expected message counts
    IntegrationTest::assert_true(stats.errors == 1, "Expected number of errors");
    IntegrationTest::assert_true(stats.warnings == 1, "Expected number of warnings");
    IntegrationTest::assert_true(stats.infos >= 3, "Expected minimum number of info messages");
    IntegrationTest::assert_true(stats.debugs >= 10, "Expected minimum number of debug messages");
    
    std::cout << "Real-world scenario stats - Errors: " << stats.errors 
              << ", Warnings: " << stats.warnings 
              << ", Infos: " << stats.infos 
              << ", Debugs: " << stats.debugs << std::endl;
}

int main() {
    std::cout << "=== CLog Desktop Integration Tests ===" << std::endl;
    
    test_file_logging_integration();
    test_multithreaded_logging();
    test_performance_integration();
    test_platform_integration();
    test_configuration_integration();
    test_real_world_scenario();
    
    return IntegrationTest::summary();
}