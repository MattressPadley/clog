#include <clog/log.hpp>
#include <clog/config.hpp>
#include <clog/platform.hpp>
#include <thread>
#include <chrono>

int main() {
    // Initialize platform-specific features
    clog::platform::init();
    
    // Print configuration info
    std::cout << "=== CLog Desktop Example ===" << std::endl;
    std::cout << "Platform: " << clog::platform::getName() << std::endl;
    std::cout << "Color support: " << (clog::platform::hasColorSupport() ? "Yes" : "No") << std::endl;
    
    // Print configuration details
    clog::config::printConfig();
    std::cout << std::endl;
    
    // Set log level to show all messages
    clog::Logger::setLevel(clog::Level::TRACE);
    
    // Demonstrate basic logging
    CLOG_ERROR("Main", "This is an error message");
    CLOG_WARN("Main", "This is a warning message");
    CLOG_INFO("Main", "This is an info message");
    CLOG_DEBUG("Main", "This is a debug message");
    CLOG_TRACE("Main", "This is a trace message");
    
    // Demonstrate formatted logging
    int count = 42;
    float value = 3.14159f;
    const char* status = "active";
    
    CLOG_INFO("Demo", "Processing %d items with value %.2f, status: %s", count, value, status);
    
    // Demonstrate different log levels
    std::cout << "\n--- Testing different log levels ---" << std::endl;
    
    clog::Logger::setLevel(clog::Level::ERROR);
    CLOG_ERROR("Level", "Only ERROR should appear");
    CLOG_WARN("Level", "This WARN should be hidden");
    CLOG_INFO("Level", "This INFO should be hidden");
    
    clog::Logger::setLevel(clog::Level::WARN);
    CLOG_ERROR("Level", "ERROR and WARN should appear");
    CLOG_WARN("Level", "WARN should appear");
    CLOG_INFO("Level", "This INFO should be hidden");
    
    clog::Logger::setLevel(clog::Level::INFO);
    CLOG_ERROR("Level", "ERROR, WARN, and INFO should appear");
    CLOG_WARN("Level", "WARN should appear");
    CLOG_INFO("Level", "INFO should appear");
    CLOG_DEBUG("Level", "This DEBUG should be hidden");
    
    // Demonstrate callback functionality
    std::cout << "\n--- Testing callback functionality ---" << std::endl;
    
    clog::Logger::setCallback([](clog::Level level, const char* tag, const char* message) {
        const char* levelStr = "";
        switch (level) {
            case clog::Level::ERROR: levelStr = "ERROR"; break;
            case clog::Level::WARN:  levelStr = "WARN "; break;
            case clog::Level::INFO:  levelStr = "INFO "; break;
            case clog::Level::DEBUG: levelStr = "DEBUG"; break;
            case clog::Level::TRACE: levelStr = "TRACE"; break;
            default: levelStr = "?????"; break;
        }
        std::cout << "[CALLBACK:" << levelStr << "] " << tag << ": " << message << std::endl;
    });
    
    CLOG_INFO("Callback", "This message should go through the callback");
    CLOG_ERROR("Callback", "Error messages also go through callback");
    
    // Reset to direct output
    clog::Logger::setCallback(nullptr);
    CLOG_INFO("Main", "Back to direct output");
    
    std::cout << "\n--- Simulating real-world usage ---" << std::endl;
    
    // Simulate a typical application workflow
    CLOG_INFO("App", "Application starting...");
    
    for (int i = 0; i < 5; i++) {
        CLOG_DEBUG("Loop", "Processing iteration %d", i);
        
        if (i == 2) {
            CLOG_WARN("Loop", "Unusual condition detected at iteration %d", i);
        }
        
        if (i == 4) {
            CLOG_ERROR("Loop", "Simulated error at iteration %d", i);
        }
        
        // Simulate some work
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    CLOG_INFO("App", "Application finished successfully");
    
    return 0;
}