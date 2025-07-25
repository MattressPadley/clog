#include <pico/stdlib.h>
#include <stdio.h>
#include <clog/log.hpp>
#include <clog/config.hpp>
#include <clog/platform.hpp>

int main() {
    // Initialize Pico SDK
    stdio_init_all();
    
    // Wait for USB serial connection
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    
    // Give a moment for terminal to be ready
    sleep_ms(1000);
    
    // Initialize platform-specific features
    clogger::platform::init();
    
    // Print configuration info
    printf("=== CLog Pico (RP2040) Example ===\n");
    printf("Platform: %s\n", clogger::platform::getName());
    printf("Color support: %s\n", clogger::platform::hasColorSupport() ? "Yes" : "No");
    
    // Print configuration details
    clogger::config::printConfig();
    printf("\n");
    
    // Log level set to TRACE at compile-time via CMake (CLOG_LEVEL=5)
    
    // Basic logging test - show all levels
    printf("\n=== Basic Logging Test ===\n");
    CLOG_ERROR("PicoTest", "This is an ERROR message");
    CLOG_WARN("PicoTest", "This is a WARN message");
    CLOG_INFO("PicoTest", "This is an INFO message");
    CLOG_DEBUG("PicoTest", "This is a DEBUG message");
    CLOG_TRACE("PicoTest", "This is a TRACE message");
    
    // Demonstrate formatted logging
    int count = 42;
    float value = 3.14159f;
    const char* status = "active";
    
    CLOG_INFO("Demo", "Processing %d items with value %.2f, status: %s", count, value, status);
    
    // Compile-time log level demonstration
    printf("\n--- Compile-time log level (all levels visible) ---\n");
    
    // Note: With compile-time CLOG_LEVEL=5 (TRACE), all messages below are visible
    // Runtime level filtering has been replaced with compile-time filtering for better performance
    CLOG_ERROR("Level", "ERROR level message (always visible)");
    CLOG_WARN("Level", "WARN level message (visible at TRACE level)");
    CLOG_INFO("Level", "INFO level message (visible at TRACE level)");
    CLOG_DEBUG("Level", "DEBUG level message (visible at TRACE level)");
    CLOG_TRACE("Level", "TRACE level message (visible at TRACE level)");
    
    // Test tag filtering
    printf("\n--- Testing Tag Filtering ---\n");
    
    // Enable only specific tags (whitelist mode)
    printf("Enabling only Database and Security tags:\n");
    clogger::Logger::enableTag("Database");
    clogger::Logger::enableTag("Security");
    
    CLOG_INFO("Database", "Query executed successfully");      // Should appear
    CLOG_INFO("Network", "Data packet received");              // Should NOT appear
    CLOG_INFO("Security", "Permission check passed");          // Should appear
    CLOG_INFO("UI", "Button clicked");                         // Should NOT appear
    
    // Reset and disable specific tags (blacklist mode)
    printf("Disabling Network and UI tags:\n");
    clogger::Logger::enableAllTags();
    clogger::Logger::disableTag("Network");
    clogger::Logger::disableTag("UI");
    
    CLOG_INFO("Database", "Transaction committed");            // Should appear
    CLOG_INFO("Network", "Connection timeout");               // Should NOT appear
    CLOG_INFO("Security", "Access granted");                  // Should appear
    CLOG_INFO("UI", "Window resized");                        // Should NOT appear
    
    // Check tag status
    printf("Database enabled: %s\n", clogger::Logger::isTagEnabled("Database") ? "Yes" : "No");
    printf("Network enabled: %s\n", clogger::Logger::isTagEnabled("Network") ? "Yes" : "No");
    
    // Reset tag filters for final demo (log level is compile-time configured)
    clogger::Logger::enableAllTags();
    
    // Simulate a typical application workflow
    printf("\n--- Simulating real-world usage ---\n");
    
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
        sleep_ms(200);
    }
    
    CLOG_INFO("App", "Pico example finished successfully");
    
    printf("\n=== Test Complete ===\n");
    printf("CLog Pico integration test completed successfully!\n");
    
    // Keep running so we can see the output
    while (true) {
        sleep_ms(1000);
    }
    
    return 0;
}