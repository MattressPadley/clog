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
    
    // Color and level format test - show all levels
    std::cout << "\n=== Color and Level Format Test ===" << std::endl;
    CLOG_ERROR("FormatTest", "This is an ERROR message - should be red");
    CLOG_WARN("FormatTest", "This is a WARN message - should be yellow");
    CLOG_INFO("FormatTest", "This is an INFO message - should be green");
    CLOG_DEBUG("FormatTest", "This is a DEBUG message - should be blue");
    CLOG_TRACE("FormatTest", "This is a TRACE message - should be gray");
    
    // Tag color configuration showcase
    std::cout << "\n=== Tag Color Configuration Showcase ===" << std::endl;
    std::cout << "Setting up custom colors for different system components..." << std::endl;
    
    // Configure colors for different system components
    clog::Logger::setTagColor("Database", clog::Color::BRIGHT_CYAN);
    clog::Logger::setTagColor("Network", clog::Color::BRIGHT_MAGENTA);  
    clog::Logger::setTagColor("Security", clog::Color::BRIGHT_RED);
    clog::Logger::setTagColor("Performance", clog::Color::BRIGHT_YELLOW);
    clog::Logger::setTagColor("UI", clog::Color::BRIGHT_GREEN);
    clog::Logger::setTagColor("FileSystem", clog::Color::BLUE);
    clog::Logger::setTagColor("Memory", clog::Color::CYAN);
    clog::Logger::setTagColor("Audio", clog::Color::MAGENTA);
    
    std::cout << "\nNow observe how each tag appears in its configured color:" << std::endl;
    
    // Demonstrate all configured tags with different log levels
    CLOG_INFO("Database", "Connection pool initialized with 10 connections");
    CLOG_WARN("Network", "High latency detected: 250ms response time");
    CLOG_ERROR("Security", "Failed login attempt from IP 192.168.1.100");
    CLOG_DEBUG("Performance", "Cache hit ratio: 94.5% (very good)");
    CLOG_INFO("UI", "Main window rendered successfully");
    CLOG_DEBUG("FileSystem", "Loaded 1,247 files from /app/data directory");
    CLOG_WARN("Memory", "Heap usage at 78% - consider cleanup");
    CLOG_ERROR("Audio", "Audio device not found - falling back to default");
    
    // Show default behavior
    CLOG_INFO("DefaultTag", "This tag has no custom color (appears in default)");
    
    std::cout << "\n--- Demonstrating Color Variety ---" << std::endl;
    std::cout << "Here's a showcase of all available colors:" << std::endl;
    
    // Showcase all available colors
    clog::Logger::setTagColor("RED", clog::Color::RED);
    clog::Logger::setTagColor("GREEN", clog::Color::GREEN);
    clog::Logger::setTagColor("YELLOW", clog::Color::YELLOW);
    clog::Logger::setTagColor("BLUE", clog::Color::BLUE);
    clog::Logger::setTagColor("MAGENTA", clog::Color::MAGENTA);
    clog::Logger::setTagColor("CYAN", clog::Color::CYAN);
    clog::Logger::setTagColor("WHITE", clog::Color::WHITE);
    clog::Logger::setTagColor("B_RED", clog::Color::BRIGHT_RED);
    clog::Logger::setTagColor("B_GREEN", clog::Color::BRIGHT_GREEN);
    clog::Logger::setTagColor("B_YELLOW", clog::Color::BRIGHT_YELLOW);
    clog::Logger::setTagColor("B_BLUE", clog::Color::BRIGHT_BLUE);
    clog::Logger::setTagColor("B_MAGENTA", clog::Color::BRIGHT_MAGENTA);
    clog::Logger::setTagColor("B_CYAN", clog::Color::BRIGHT_CYAN);
    clog::Logger::setTagColor("B_WHITE", clog::Color::BRIGHT_WHITE);
    
    CLOG_INFO("RED", "Regular red color");
    CLOG_INFO("GREEN", "Regular green color");
    CLOG_INFO("YELLOW", "Regular yellow color");
    CLOG_INFO("BLUE", "Regular blue color");
    CLOG_INFO("MAGENTA", "Regular magenta color");
    CLOG_INFO("CYAN", "Regular cyan color");
    CLOG_INFO("WHITE", "Regular white color");
    CLOG_INFO("B_RED", "Bright red color");
    CLOG_INFO("B_GREEN", "Bright green color");
    CLOG_INFO("B_YELLOW", "Bright yellow color");
    CLOG_INFO("B_BLUE", "Bright blue color");
    CLOG_INFO("B_MAGENTA", "Bright magenta color");
    CLOG_INFO("B_CYAN", "Bright cyan color");
    CLOG_INFO("B_WHITE", "Bright white color");
    
    std::cout << "\n--- Dynamic Color Management ---" << std::endl;
    CLOG_INFO("TempTag", "This tag will change color...");
    
    clog::Logger::setTagColor("TempTag", clog::Color::BRIGHT_RED);
    CLOG_INFO("TempTag", "Now I'm bright red!");
    
    clog::Logger::setTagColor("TempTag", clog::Color::BRIGHT_GREEN);
    CLOG_INFO("TempTag", "Now I'm bright green!");
    
    clog::Logger::clearTagColor("TempTag");
    CLOG_INFO("TempTag", "Back to default color after clearing");
    
    // Demonstrate basic logging
    std::cout << "\n=== Basic Logging Examples ===" << std::endl;
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
    
    std::cout << "\n--- Testing Tag Filtering Feature ---" << std::endl;
    
    // Demonstrate tag filtering capabilities
    std::cout << "Testing new tag filtering functionality..." << std::endl;
    
    // Start with all tags enabled (default behavior)
    clog::Logger::enableAllTags();
    std::cout << "\n1. All tags enabled (default):" << std::endl;
    CLOG_INFO("Database", "Connection established");
    CLOG_INFO("Network", "Socket opened");
    CLOG_INFO("Security", "User authenticated");
    
    // Enable only specific tags (whitelist mode)
    std::cout << "\n2. Only Database and Security tags enabled:" << std::endl;
    clog::Logger::enableTag("Database");  // This switches to whitelist mode
    clog::Logger::enableTag("Security");
    
    CLOG_INFO("Database", "Query executed successfully");      // Should appear
    CLOG_INFO("Network", "Data packet received");              // Should NOT appear
    CLOG_INFO("Security", "Permission check passed");          // Should appear
    CLOG_INFO("UI", "Button clicked");                         // Should NOT appear
    
    // Disable specific tags (blacklist mode)
    std::cout << "\n3. All tags except Network and UI enabled:" << std::endl;
    clog::Logger::enableAllTags();         // Reset to allow all
    clog::Logger::disableTag("Network");   // This switches to blacklist mode
    clog::Logger::disableTag("UI");
    
    CLOG_INFO("Database", "Transaction committed");            // Should appear
    CLOG_INFO("Network", "Connection timeout");               // Should NOT appear
    CLOG_INFO("Security", "Access granted");                  // Should appear
    CLOG_INFO("UI", "Window resized");                        // Should NOT appear
    
    // Disable all tags, then enable specific ones
    std::cout << "\n4. Start with no tags, then enable Database only:" << std::endl;
    clog::Logger::disableAllTags();        // Disable everything
    
    CLOG_INFO("Database", "Should not appear");
    CLOG_INFO("Security", "Should not appear");
    
    clog::Logger::enableTag("Database");   // Enable just Database
    CLOG_INFO("Database", "Now Database appears");             // Should appear
    CLOG_INFO("Security", "Security still hidden");           // Should NOT appear
    
    // Check tag status
    std::cout << "\n5. Checking tag status programmatically:" << std::endl;
    std::cout << "Database enabled: " << (clog::Logger::isTagEnabled("Database") ? "Yes" : "No") << std::endl;
    std::cout << "Security enabled: " << (clog::Logger::isTagEnabled("Security") ? "Yes" : "No") << std::endl;
    std::cout << "Network enabled: " << (clog::Logger::isTagEnabled("Network") ? "Yes" : "No") << std::endl;
    
    // Demonstrate tag filtering with different log levels
    std::cout << "\n6. Tag filtering combined with log levels:" << std::endl;
    clog::Logger::setLevel(clog::Level::WARN);  // Only ERROR and WARN
    clog::Logger::enableAllTags();              // Allow all tags
    clog::Logger::disableTag("Debug");          // But disable Debug tag
    
    CLOG_ERROR("System", "Critical error occurred");           // Should appear (ERROR level, tag allowed)
    CLOG_WARN("System", "Warning message");                    // Should appear (WARN level, tag allowed)
    CLOG_INFO("System", "Info message");                       // Should NOT appear (INFO > WARN level)
    CLOG_ERROR("Debug", "Debug error");                        // Should NOT appear (tag disabled)
    
    // Reset for next demo
    clog::Logger::setLevel(clog::Level::INFO);
    clog::Logger::enableAllTags();
    
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