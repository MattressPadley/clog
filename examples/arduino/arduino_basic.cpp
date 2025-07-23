#include <clog/log.hpp>

// Global variables for demonstration
int sensorValue = 0;
bool systemStatus = true;
unsigned long lastUpdate = 0;

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    while (!Serial) {
        ; // Wait for serial port to connect (needed for native USB)
    }
    
    // Initialize CLog
    clog::Logger::init();
    clog::Logger::setLevel(clog::Level::DEBUG);  // Show debug messages
    
    CLOG_INFO("System", "Arduino CLog example starting...");
    CLOG_DEBUG("System", "Debug logging enabled");
    
    // Simulate sensor initialization
    CLOG_INFO("Hardware", "Initializing sensors...");
    delay(100);
    CLOG_INFO("Hardware", "All sensors initialized successfully");
    
    // Setup complete
    CLOG_INFO("System", "Setup complete, entering main loop");
}

void loop() {
    unsigned long currentTime = millis();
    
    // Update sensor reading every 2 seconds
    if (currentTime - lastUpdate >= 2000) {
        lastUpdate = currentTime;
        
        // Simulate sensor reading
        sensorValue = analogRead(A0);
        CLOG_DEBUG("Sensor", "Raw ADC reading: %d", sensorValue);
        
        // Convert to meaningful value (e.g., voltage)
        float voltage = sensorValue * (5.0 / 1023.0);
        CLOG_INFO("Sensor", "Sensor voltage: %.2fV", voltage);
        
        // Check for warning conditions
        if (voltage > 4.5) {
            CLOG_WARN("Sensor", "High voltage detected: %.2fV", voltage);
        } else if (voltage < 0.5) {
            CLOG_WARN("Sensor", "Low voltage detected: %.2fV", voltage);
        }
        
        // Simulate error condition (rarely)
        if (sensorValue == 0 && random(100) < 5) {  // 5% chance
            CLOG_ERROR("Sensor", "Sensor reading failed - check connections");
            systemStatus = false;
        } else {
            systemStatus = true;
        }
        
        // Log system status periodically
        static int statusCounter = 0;
        statusCounter++;
        if (statusCounter >= 5) {  // Every 10 seconds
            statusCounter = 0;
            CLOG_INFO("Status", "System %s, uptime: %lu ms", 
                     systemStatus ? "OK" : "ERROR", currentTime);
        }
    }
    
    // Simulate other periodic tasks
    static unsigned long taskTimer = 0;
    if (currentTime - taskTimer >= 5000) {  // Every 5 seconds
        taskTimer = currentTime;
        
        CLOG_TRACE("Task", "Performing maintenance task");
        
        // Simulate memory usage check
        int freeMemory = getFreeMemory();
        if (freeMemory < 500) {
            CLOG_WARN("Memory", "Low memory: %d bytes free", freeMemory);
        } else {
            CLOG_DEBUG("Memory", "Memory OK: %d bytes free", freeMemory);
        }
    }
    
    // Small delay to prevent overwhelming the serial output
    delay(10);
}

// Helper function to get free memory (simplified)
int getFreeMemory() {
    // This is a simplified version - real implementation would be more complex
    return random(200, 1500);  // Simulate variable free memory
}