#include <clog/log.hpp>
#include <clog/platform.hpp>
#include <clog/config.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <chrono>
#include <thread>

// Embedded simulation test framework
class EmbeddedSimTest {
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
        std::cout << "\n=== Embedded Simulation Test Summary ===" << std::endl;
        std::cout << "Tests run: " << tests_run << std::endl;
        std::cout << "Tests passed: " << tests_passed << std::endl;
        std::cout << "Tests failed: " << (tests_run - tests_passed) << std::endl;
        return (tests_run == tests_passed) ? 0 : 1;
    }
};

int EmbeddedSimTest::tests_run = 0;
int EmbeddedSimTest::tests_passed = 0;

// Simulate memory-constrained environment
class MemoryConstrainedLogger {
private:
    static const size_t MAX_LOG_ENTRIES = 10;  // Very small buffer
    struct LogEntry {
        char tag[16];
        char message[64];
        int level;
    };
    
    LogEntry log_buffer[MAX_LOG_ENTRIES];
    size_t current_index = 0;
    size_t total_logs = 0;
    
public:
    void addLog(clog::Level level, const char* tag, const char* message) {
        // Simulate embedded circular buffer behavior
        LogEntry& entry = log_buffer[current_index];
        entry.level = static_cast<int>(level);
        
        // Truncate if necessary (embedded behavior)
        strncpy(entry.tag, tag, sizeof(entry.tag) - 1);
        entry.tag[sizeof(entry.tag) - 1] = '\0';
        
        strncpy(entry.message, message, sizeof(entry.message) - 1);
        entry.message[sizeof(entry.message) - 1] = '\0';
        
        current_index = (current_index + 1) % MAX_LOG_ENTRIES;
        total_logs++;
    }
    
    size_t getTotalLogs() const { return total_logs; }
    size_t getBufferSize() const { return MAX_LOG_ENTRIES; }
    
    LogEntry getEntry(size_t index) const {
        if (index < MAX_LOG_ENTRIES) {
            return log_buffer[index];
        }
        return LogEntry{};
    }
};

void test_memory_constrained_logging() {
    std::cout << "\n--- Testing Memory Constrained Logging ---" << std::endl;
    
    MemoryConstrainedLogger embedded_logger;
    
    clog::Logger::setCallback([&embedded_logger](clog::Level level, const char* tag, const char* message) {
        embedded_logger.addLog(level, tag, message);
    });
    
    clog::Logger::setLevel(clog::Level::DEBUG);
    
    // Simulate typical embedded logging scenario
    CLOG_INFO("System", "Boot");
    CLOG_DEBUG("Init", "GPIO setup");
    CLOG_DEBUG("Init", "Timer setup"); 
    CLOG_INFO("System", "Ready");
    
    // Simulate sensor readings
    for (int i = 0; i < 8; i++) {
        CLOG_DEBUG("Sensor", "Reading %d", i);
    }
    
    CLOG_WARN("Sensor", "High temp");
    CLOG_ERROR("System", "Fault");
    
    clog::Logger::setCallback(nullptr);
    
    EmbeddedSimTest::assert_true(embedded_logger.getTotalLogs() == 14, "All messages logged");
    EmbeddedSimTest::assert_true(embedded_logger.getBufferSize() == 10, "Circular buffer size correct");
    
    // Verify circular buffer behavior (should overwrite old entries)
    auto last_entry = embedded_logger.getEntry(0);  // Should be overwritten
    EmbeddedSimTest::assert_true(strlen(last_entry.message) > 0, "Circular buffer contains data");
}

void test_low_level_performance() {
    std::cout << "\n--- Testing Low-level Performance ---" << std::endl;
    
    // Measure performance with minimal overhead
    volatile int dummy_counter = 0;
    
    clog::Logger::setCallback([&dummy_counter](clog::Level level, const char* tag, const char* message) {
        // Simulate very fast embedded logging (just increment counter)
        dummy_counter++;
    });
    
    clog::Logger::setLevel(clog::Level::DEBUG);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Fast logging loop (simulate real-time constraints)
    for (int i = 0; i < 100; i++) {
        CLOG_DEBUG("Fast", "Loop %d", i);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    clog::Logger::setCallback(nullptr);
    
    EmbeddedSimTest::assert_true(dummy_counter == 100, "All fast messages processed");
    EmbeddedSimTest::assert_true(duration.count() < 50000, "Fast logging completed in < 50ms");  // 50ms for 100 messages
    
    std::cout << "100 messages in " << duration.count() << " microseconds" << std::endl;
    std::cout << "Average: " << (duration.count() / 100.0) << " microseconds per message" << std::endl;
}

void test_compile_time_filtering() {
    std::cout << "\n--- Testing Compile-time Filtering Simulation ---" << std::endl;
    
    int message_count = 0;
    
    clog::Logger::setCallback([&message_count](clog::Level level, const char* tag, const char* message) {
        message_count++;
    });
    
    // Test different log levels to simulate compile-time filtering
    clog::Logger::setLevel(clog::Level::ERROR);
    
    // These should be filtered out at runtime (in embedded, they'd be filtered at compile-time)
    CLOG_ERROR("Filter", "Error message");   // Should appear
    CLOG_WARN("Filter", "Warning message");  // Should be filtered
    CLOG_INFO("Filter", "Info message");     // Should be filtered
    CLOG_DEBUG("Filter", "Debug message");   // Should be filtered
    
    EmbeddedSimTest::assert_true(message_count == 1, "ERROR level filtering works");
    
    message_count = 0;
    clog::Logger::setLevel(clog::Level::INFO);
    
    CLOG_ERROR("Filter", "Error message");   // Should appear
    CLOG_WARN("Filter", "Warning message");  // Should appear
    CLOG_INFO("Filter", "Info message");     // Should appear
    CLOG_DEBUG("Filter", "Debug message");   // Should be filtered
    
    EmbeddedSimTest::assert_true(message_count == 3, "INFO level filtering works");
    
    clog::Logger::setCallback(nullptr);
}

void test_buffer_overflow_handling() {
    std::cout << "\n--- Testing Buffer Overflow Handling ---" << std::endl;
    
    std::vector<std::string> captured_messages;
    
    clog::Logger::setCallback([&captured_messages](clog::Level level, const char* tag, const char* message) {
        captured_messages.push_back(message);
    });
    
    clog::Logger::setLevel(clog::Level::INFO);
    
    // Test with very long message (should be truncated)
    std::string very_long_message(2000, 'A');  // 2000 characters
    CLOG_INFO("BufferTest", "%s", very_long_message.c_str());
    
    // Test with many format arguments
    CLOG_INFO("BufferTest", "Many args: %d %d %d %d %d %d %d %d %d %d", 
              1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    
    clog::Logger::setCallback(nullptr);
    
    EmbeddedSimTest::assert_true(captured_messages.size() == 2, "Messages captured despite overflow");
    EmbeddedSimTest::assert_true(captured_messages[0].length() < 2000, "Long message was truncated");
    EmbeddedSimTest::assert_true(captured_messages[1].find("Many args") != std::string::npos, "Format message handled");
    
    std::cout << "Long message truncated to " << captured_messages[0].length() << " characters" << std::endl;
}

void test_real_time_constraints() {
    std::cout << "\n--- Testing Real-time Constraints ---" << std::endl;
    
    // Simulate real-time system with strict timing requirements
    const int ITERATIONS = 50;
    const int MAX_MICROSECONDS_PER_ITERATION = 200;  // 200us max per iteration
    
    std::vector<std::chrono::microseconds> iteration_times;
    
    clog::Logger::setCallback([](clog::Level level, const char* tag, const char* message) {
        // Simulate very fast embedded output (e.g., DMA to UART buffer)
        volatile int dummy = 0;
        for (int i = 0; i < 5; i++) dummy += i;  // Minimal processing
    });
    
    clog::Logger::setLevel(clog::Level::INFO);
    
    for (int i = 0; i < ITERATIONS; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        
        // Simulate real-time critical section with logging
        CLOG_DEBUG("RT", "Iteration %d", i);
        
        // Simulate some real-time work
        volatile int work = 0;
        for (int j = 0; j < 100; j++) work += j;
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        iteration_times.push_back(duration);
    }
    
    clog::Logger::setCallback(nullptr);
    
    // Analyze timing results
    int violations = 0;
    auto max_time = std::chrono::microseconds(0);
    auto total_time = std::chrono::microseconds(0);
    
    for (const auto& time : iteration_times) {
        total_time += time;
        if (time > max_time) max_time = time;
        if (time.count() > MAX_MICROSECONDS_PER_ITERATION) violations++;
    }
    
    auto avg_time = total_time / ITERATIONS;
    
    EmbeddedSimTest::assert_true(violations < (ITERATIONS / 10), "< 10% real-time violations");  // Allow some violations
    
    std::cout << "Real-time analysis:" << std::endl;
    std::cout << "  Average time: " << avg_time.count() << " microseconds" << std::endl;
    std::cout << "  Max time: " << max_time.count() << " microseconds" << std::endl;
    std::cout << "  Violations: " << violations << "/" << ITERATIONS << std::endl;
}

void test_interrupt_context_simulation() {
    std::cout << "\n--- Testing Interrupt Context Simulation ---" << std::endl;
    
    // Simulate logging from interrupt context (very fast, minimal processing)
    std::vector<std::string> interrupt_logs;
    
    clog::Logger::setCallback([&interrupt_logs](clog::Level level, const char* tag, const char* message) {
        // In real embedded system, this would go to a lock-free ring buffer
        interrupt_logs.push_back(std::string(tag) + ":" + message);
    });
    
    clog::Logger::setLevel(clog::Level::ERROR);  // Only critical messages in interrupts
    
    // Simulate multiple interrupt events
    for (int i = 0; i < 10; i++) {
        // Only log critical errors from interrupt context
        if (i == 3 || i == 7) {  // Simulate error conditions
            CLOG_ERROR("IRQ", "Critical error %d", i);
        }
    }
    
    clog::Logger::setCallback(nullptr);
    
    EmbeddedSimTest::assert_true(interrupt_logs.size() == 2, "Only critical interrupt messages logged");
    EmbeddedSimTest::assert_true(interrupt_logs[0].find("Critical error 3") != std::string::npos, 
                                "First interrupt error correct");
    EmbeddedSimTest::assert_true(interrupt_logs[1].find("Critical error 7") != std::string::npos, 
                                "Second interrupt error correct");
}

int main() {
    std::cout << "=== CLog Embedded Simulation Tests ===" << std::endl;
    std::cout << "Simulating embedded system constraints on desktop platform" << std::endl;
    
    test_memory_constrained_logging();
    test_low_level_performance();
    test_compile_time_filtering();
    test_buffer_overflow_handling();
    test_real_time_constraints();
    test_interrupt_context_simulation();
    
    return EmbeddedSimTest::summary();
}