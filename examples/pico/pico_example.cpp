#include <clog/log.hpp>
#include <clog/config.hpp>
#include <clog/platform.hpp>
#include <pico/stdlib.h>
#include <stdio.h>

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
    
    // Set log level to show all messages
    clogger::Logger::setLevel(clogger::Level::TRACE);
    
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
    
    // Test different log levels
    printf("\n--- Testing different log levels ---\n");
    
    clogger::Logger::setLevel(clogger::Level::ERROR);
    CLOG_ERROR("Level", "Only ERROR should appear");
    CLOG_WARN("Level", "This WARN should be hidden");
    CLOG_INFO("Level", "This INFO should be hidden");
    
    clogger::Logger::setLevel(clogger::Level::WARN);
    CLOG_ERROR("Level", "ERROR and WARN should appear");
    CLOG_WARN("Level", "WARN should appear");
    CLOG_INFO("Level", "This INFO should be hidden");
    
    clogger::Logger::setLevel(clogger::Level::INFO);
    CLOG_ERROR("Level", "ERROR, WARN, and INFO should appear");
    CLOG_WARN("Level", "WARN should appear");
    CLOG_INFO("Level", "INFO should appear");
    CLOG_DEBUG("Level", "This DEBUG should be hidden");
    
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
    
    // Reset for final demo
    clogger::Logger::setLevel(clogger::Level::INFO);
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