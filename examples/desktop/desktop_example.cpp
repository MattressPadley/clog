// Define library name for compile-time identification
#define CLOG_LIBRARY_NAME "DesktopExample"

#include <clog/log.hpp>
#include <thread>
#include <chrono>

int main() {
    // Initialize platform-specific features
    clogger::platform::init();
    
    // Print configuration info
    std::cout << "=== CLog Desktop Example ===" << std::endl;
    std::cout << "Platform: " << clogger::platform::getName() << std::endl;
    std::cout << "Color support: " << (clogger::platform::hasColorSupport() ? "Yes" : "No") << std::endl;
    
    // Print configuration details
    clogger::config::printConfig();
    std::cout << std::endl;
    
    // Log level set to TRACE at compile-time via CMake (CLOG_LEVEL=5)
    
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
    clogger::Logger::setTagColor("Database", clogger::Color::CLOG_BRIGHT_CYAN);
    clogger::Logger::setTagColor("Network", clogger::Color::CLOG_BRIGHT_MAGENTA);  
    clogger::Logger::setTagColor("Security", clogger::Color::CLOG_BRIGHT_RED);
    clogger::Logger::setTagColor("Performance", clogger::Color::CLOG_BRIGHT_YELLOW);
    clogger::Logger::setTagColor("UI", clogger::Color::CLOG_BRIGHT_GREEN);
    clogger::Logger::setTagColor("FileSystem", clogger::Color::CLOG_BLUE);
    clogger::Logger::setTagColor("Memory", clogger::Color::CLOG_CYAN);
    clogger::Logger::setTagColor("Audio", clogger::Color::CLOG_MAGENTA);
    
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
    clogger::Logger::setTagColor("RED", clogger::Color::CLOG_RED);
    clogger::Logger::setTagColor("GREEN", clogger::Color::CLOG_GREEN);
    clogger::Logger::setTagColor("YELLOW", clogger::Color::CLOG_YELLOW);
    clogger::Logger::setTagColor("BLUE", clogger::Color::CLOG_BLUE);
    clogger::Logger::setTagColor("MAGENTA", clogger::Color::CLOG_MAGENTA);
    clogger::Logger::setTagColor("CYAN", clogger::Color::CLOG_CYAN);
    clogger::Logger::setTagColor("WHITE", clogger::Color::CLOG_WHITE);
    clogger::Logger::setTagColor("B_RED", clogger::Color::CLOG_BRIGHT_RED);
    clogger::Logger::setTagColor("B_GREEN", clogger::Color::CLOG_BRIGHT_GREEN);
    clogger::Logger::setTagColor("B_YELLOW", clogger::Color::CLOG_BRIGHT_YELLOW);
    clogger::Logger::setTagColor("B_BLUE", clogger::Color::CLOG_BRIGHT_BLUE);
    clogger::Logger::setTagColor("B_MAGENTA", clogger::Color::CLOG_BRIGHT_MAGENTA);
    clogger::Logger::setTagColor("B_CYAN", clogger::Color::CLOG_BRIGHT_CYAN);
    clogger::Logger::setTagColor("B_WHITE", clogger::Color::CLOG_BRIGHT_WHITE);
    
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
    
    clogger::Logger::setTagColor("TempTag", clogger::Color::CLOG_BRIGHT_RED);
    CLOG_INFO("TempTag", "Now I'm bright red!");
    
    clogger::Logger::setTagColor("TempTag", clogger::Color::CLOG_BRIGHT_GREEN);
    CLOG_INFO("TempTag", "Now I'm bright green!");
    
    clogger::Logger::clearTagColor("TempTag");
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
    
    // Compile-time log level demonstration
    std::cout << "\n--- Compile-time log level (all levels visible) ---" << std::endl;
    
    // Note: With compile-time CLOG_LEVEL=5 (TRACE), all messages below are visible
    // Runtime level filtering has been replaced with compile-time filtering for better performance
    CLOG_ERROR("Level", "ERROR level message (always visible)");
    CLOG_WARN("Level", "WARN level message (visible at TRACE level)");
    CLOG_INFO("Level", "INFO level message (visible at TRACE level)");
    CLOG_DEBUG("Level", "DEBUG level message (visible at TRACE level)");
    CLOG_TRACE("Level", "TRACE level message (visible at TRACE level)");
    
    // Demonstrate callback functionality
    std::cout << "\n--- Testing callback functionality ---" << std::endl;
    
    clogger::Logger::setCallback([](clogger::Level level, const char* tag, const char* message, const char* libraryName) {
        const char* levelStr = "";
        switch (level) {
            case clogger::Level::ERROR: levelStr = "ERROR"; break;
            case clogger::Level::WARN:  levelStr = "WARN "; break;
            case clogger::Level::INFO:  levelStr = "INFO "; break;
            case clogger::Level::DEBUG: levelStr = "DEBUG"; break;
            case clogger::Level::TRACE: levelStr = "TRACE"; break;
            default: levelStr = "?????"; break;
        }
        std::cout << "[CALLBACK:" << levelStr << "] ";
        if (libraryName && libraryName[0] != '\0') {
            std::cout << "[" << libraryName << "]";
        }
        std::cout << tag << ": " << message << std::endl;
    });
    
    CLOG_INFO("Callback", "This message should go through the callback");
    CLOG_ERROR("Callback", "Error messages also go through callback");
    
    // Reset to direct output
    clogger::Logger::setCallback(nullptr);
    CLOG_INFO("Main", "Back to direct output");
    
    std::cout << "\n--- Testing Library Tagging Feature ---" << std::endl;
    
    // Demonstrate compile-time library identification
    std::cout << "Demonstrating compile-time library tagging functionality..." << std::endl;
    std::cout << "This example uses #define CLOG_LIBRARY_NAME \"DesktopExample\"" << std::endl;
    
    // Step 1: Library logs with tags disabled (default)
    std::cout << "\n1. Library logs with library tags disabled by default:" << std::endl;
    CLOG_INFO("Database", "Connection established");
    CLOG_INFO("Network", "Socket opened");
    
    // Step 2: Parent application enables library tags
    std::cout << "\n2. Parent application enables library tags:" << std::endl;
    clogger::Logger::enableLibraryTags(true);
    CLOG_INFO("Database", "Query executed successfully");
    CLOG_INFO("Network", "Data packet received");
    CLOG_ERROR("Security", "Authentication failed");
    
    // Step 3: Demonstrate library colors
    std::cout << "\n3. Configure colors for library names:" << std::endl;
    clogger::Logger::setLibraryColor("DesktopExample", clogger::Color::CLOG_BRIGHT_CYAN);
    clogger::Logger::setTagColor("Database", clogger::Color::CLOG_BRIGHT_GREEN);
    clogger::Logger::setTagColor("Network", clogger::Color::CLOG_BRIGHT_MAGENTA);
    clogger::Logger::setTagColor("Security", clogger::Color::CLOG_BRIGHT_RED);
    
    CLOG_INFO("Database", "Colored library and tag names");
    CLOG_INFO("Network", "Library: cyan, Tag: magenta");
    CLOG_ERROR("Security", "Library: cyan, Tag: red");
    
    // Step 4: Note about multiple libraries
    std::cout << "\n4. Multiple libraries scenario:" << std::endl;
    std::cout << "   In a real multi-library scenario, each library would define" << std::endl;
    std::cout << "   its own CLOG_LIBRARY_NAME before including <clog/log.hpp>" << std::endl;
    std::cout << "   This ensures each library maintains its own identity." << std::endl;
    
    CLOG_INFO("Connection", "This example library shows DesktopExample");
    CLOG_INFO("HTTP", "All logs from this file show the same library name");
    CLOG_WARN("TCP", "Because they're compiled with the same CLOG_LIBRARY_NAME");
    
    // Step 5: Disable library tags (back to regular format)
    std::cout << "\n5. Parent disables library tags (back to regular format):" << std::endl;
    clogger::Logger::enableLibraryTags(false);
    CLOG_INFO("Database", "Still showing operations");
    CLOG_INFO("Network", "Still showing operations");
    
    // Step 6: Show library context info
    std::cout << "\n6. Library context information:" << std::endl;
    std::cout << "Compile-time library name: 'DesktopExample' (embedded in binary)" << std::endl;
    std::cout << "Library tags enabled: " << (clogger::Logger::isLibraryTagsEnabled() ? "Yes" : "No") << std::endl;
    
    // Reset for next demo
    clogger::Logger::enableLibraryTags(true);
    clogger::Logger::setLibraryColor("CLog", clogger::Color::CLOG_BRIGHT_WHITE);
    
    std::cout << "\n--- Testing Tag Filtering Feature ---" << std::endl;
    
    // Demonstrate tag filtering capabilities
    std::cout << "Testing new tag filtering functionality..." << std::endl;
    
    // Start with all tags enabled (default behavior)
    clogger::Logger::enableAllTags();
    std::cout << "\n1. All tags enabled (default):" << std::endl;
    CLOG_INFO("Database", "Connection established");
    CLOG_INFO("Network", "Socket opened");
    CLOG_INFO("Security", "User authenticated");
    
    // Enable only specific tags (whitelist mode)
    std::cout << "\n2. Only Database and Security tags enabled:" << std::endl;
    clogger::Logger::enableTag("Database");  // This switches to whitelist mode
    clogger::Logger::enableTag("Security");
    
    CLOG_INFO("Database", "Query executed successfully");      // Should appear
    CLOG_INFO("Network", "Data packet received");              // Should NOT appear
    CLOG_INFO("Security", "Permission check passed");          // Should appear
    CLOG_INFO("UI", "Button clicked");                         // Should NOT appear
    
    // Disable specific tags (blacklist mode)
    std::cout << "\n3. All tags except Network and UI enabled:" << std::endl;
    clogger::Logger::enableAllTags();         // Reset to allow all
    clogger::Logger::disableTag("Network");   // This switches to blacklist mode
    clogger::Logger::disableTag("UI");
    
    CLOG_INFO("Database", "Transaction committed");            // Should appear
    CLOG_INFO("Network", "Connection timeout");               // Should NOT appear
    CLOG_INFO("Security", "Access granted");                  // Should appear
    CLOG_INFO("UI", "Window resized");                        // Should NOT appear
    
    // Disable all tags, then enable specific ones
    std::cout << "\n4. Start with no tags, then enable Database only:" << std::endl;
    clogger::Logger::disableAllTags();        // Disable everything
    
    CLOG_INFO("Database", "Should not appear");
    CLOG_INFO("Security", "Should not appear");
    
    clogger::Logger::enableTag("Database");   // Enable just Database
    CLOG_INFO("Database", "Now Database appears");             // Should appear
    CLOG_INFO("Security", "Security still hidden");           // Should NOT appear
    
    // Check tag status
    std::cout << "\n5. Checking tag status programmatically:" << std::endl;
    std::cout << "Database enabled: " << (clogger::Logger::isTagEnabled("Database") ? "Yes" : "No") << std::endl;
    std::cout << "Security enabled: " << (clogger::Logger::isTagEnabled("Security") ? "Yes" : "No") << std::endl;
    std::cout << "Network enabled: " << (clogger::Logger::isTagEnabled("Network") ? "Yes" : "No") << std::endl;
    
    // Demonstrate tag filtering with compile-time log levels
    std::cout << "\n6. Tag filtering (all log levels visible due to compile-time TRACE level):" << std::endl;
    clogger::Logger::enableAllTags();              // Allow all tags
    clogger::Logger::disableTag("Debug");          // But disable Debug tag
    
    CLOG_ERROR("System", "Critical error occurred (tag allowed)");           // Should appear
    CLOG_WARN("System", "Warning message (tag allowed)");                    // Should appear
    CLOG_INFO("System", "Info message (tag allowed)");                       // Should appear (compile-time TRACE level)
    CLOG_ERROR("Debug", "Debug error (tag disabled)");                        // Should NOT appear (tag disabled)
    
    // Reset tag filters for next demo
    clogger::Logger::enableAllTags();
    
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