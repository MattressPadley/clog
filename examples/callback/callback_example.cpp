#include <clog/log.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <cstdlib>

// Example of integrating CLog into a parent application with custom logging system

// Custom log entry structure for the parent application
struct LogEntry {
    std::string timestamp;
    std::string level;
    std::string category;  
    std::string tag;
    std::string message;
};

// Parent application's logging system
class ApplicationLogger {
private:
    std::vector<LogEntry> logHistory;
    bool enableConsoleOutput = true;
    
public:
    // Method to receive logs from CLog
    void handleCLogMessage(clog::Level level, const char* tag, const char* message) {
        LogEntry entry;
        entry.timestamp = getCurrentTimestamp();
        entry.level = clogLevelToString(level);
        entry.category = determineCategory(tag);
        entry.tag = tag;
        entry.message = message;
        
        // Store in history
        logHistory.push_back(entry);
        
        // Limit history size
        if (logHistory.size() > 1000) {
            logHistory.erase(logHistory.begin());
        }
        
        // Output to console if enabled
        if (enableConsoleOutput) {
            outputToConsole(entry);
        }
        
        // Could also write to file, send to remote server, etc.
        writeToFile(entry);
    }
    
    // Get recent log entries
    std::vector<LogEntry> getRecentLogs(size_t count = 10) {
        size_t start = logHistory.size() > count ? logHistory.size() - count : 0;
        return std::vector<LogEntry>(logHistory.begin() + start, logHistory.end());
    }
    
    // Filter logs by level
    std::vector<LogEntry> getLogsByLevel(const std::string& level) {
        std::vector<LogEntry> filtered;
        for (const auto& entry : logHistory) {
            if (entry.level == level) {
                filtered.push_back(entry);
            }
        }
        return filtered;
    }
    
    // Filter logs by category
    std::vector<LogEntry> getLogsByCategory(const std::string& category) {
        std::vector<LogEntry> filtered;
        for (const auto& entry : logHistory) {
            if (entry.category == category) {
                filtered.push_back(entry);
            }
        }
        return filtered;
    }
    
    void enableConsole(bool enable) { enableConsoleOutput = enable; }
    
private:
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    std::string clogLevelToString(clog::Level level) {
        switch (level) {
            case clog::Level::ERROR: return "ERROR";
            case clog::Level::WARN:  return "WARN";
            case clog::Level::INFO:  return "INFO";
            case clog::Level::DEBUG: return "DEBUG";
            case clog::Level::TRACE: return "TRACE";
            default: return "UNKNOWN";
        }
    }
    
    std::string determineCategory(const std::string& tag) {
        // Categorize based on tag patterns
        if (tag.find("Network") != std::string::npos || 
            tag.find("HTTP") != std::string::npos ||
            tag.find("Socket") != std::string::npos) {
            return "NETWORK";
        } else if (tag.find("Database") != std::string::npos ||
                   tag.find("SQL") != std::string::npos) {
            return "DATABASE";
        } else if (tag.find("Auth") != std::string::npos ||
                   tag.find("Security") != std::string::npos) {
            return "SECURITY";
        } else if (tag.find("Hardware") != std::string::npos ||
                   tag.find("Sensor") != std::string::npos ||
                   tag.find("Motor") != std::string::npos) {
            return "HARDWARE";
        } else {
            return "GENERAL";
        }
    }
    
    void outputToConsole(const LogEntry& entry) {
        // Color coding for different categories
        std::string color;
        if (entry.level == "ERROR") color = "\033[91m";      // Red
        else if (entry.level == "WARN") color = "\033[93m";  // Yellow
        else if (entry.level == "INFO") color = "\033[92m";  // Green
        else if (entry.level == "DEBUG") color = "\033[94m"; // Blue
        else if (entry.level == "TRACE") color = "\033[90m"; // Gray
        
        std::cout << color << "[" << entry.timestamp << "][" << entry.category 
                  << "][" << entry.level << "] " << entry.tag << ": " 
                  << entry.message << "\033[0m" << std::endl;
    }
    
    void writeToFile(const LogEntry& entry) {
        // In a real application, this would write to a log file
        // For this example, we'll just simulate it
        static int fileWriteCount = 0;
        fileWriteCount++;
        
        if (fileWriteCount % 10 == 0) {
            std::cout << "  [File] Wrote " << fileWriteCount << " log entries to file" << std::endl;
        }
    }
};

// Global logger instance
ApplicationLogger appLogger;

// Callback function to bridge CLog to ApplicationLogger
void clogCallback(clog::Level level, const char* tag, const char* message) {
    appLogger.handleCLogMessage(level, tag, message);
}

// Simulate different subsystems using CLog
void simulateNetworkSubsystem() {
    CLOG_INFO("NetworkManager", "Initializing network subsystem");
    CLOG_DEBUG("HTTPClient", "Setting up HTTP client with timeout 30s");
    
    // Simulate some network operations
    for (int i = 0; i < 3; i++) {
        CLOG_DEBUG("HTTPClient", "Sending request %d", i + 1);
        
        if (i == 1) {
            CLOG_WARN("HTTPClient", "Request %d took longer than expected", i + 1);
        }
    }
    
    CLOG_INFO("NetworkManager", "Network subsystem ready");
}

void simulateHardwareSubsystem() {
    CLOG_INFO("HardwareManager", "Initializing hardware subsystem");
    
    // Simulate hardware initialization
    CLOG_DEBUG("MotorController", "Calibrating motors...");
    CLOG_DEBUG("SensorArray", "Reading initial sensor values");
    
    // Simulate sensor readings
    for (int i = 0; i < 5; i++) {
        int sensorValue = 100 + (rand() % 50);
        CLOG_TRACE("SensorArray", "Sensor %d reading: %d", i, sensorValue);
        
        if (sensorValue > 140) {
            CLOG_WARN("SensorArray", "Sensor %d reading high: %d", i, sensorValue);
        }
    }
    
    // Simulate an error condition
    CLOG_ERROR("MotorController", "Motor 2 overcurrent detected - disabling");
    
    CLOG_INFO("HardwareManager", "Hardware subsystem initialized with warnings");
}

void simulateDatabaseSubsystem() {
    CLOG_INFO("DatabaseManager", "Connecting to database");
    CLOG_DEBUG("SQLConnector", "Using connection string: localhost:5432/mydb");
    
    // Simulate database operations
    CLOG_DEBUG("SQLConnector", "Executing query: SELECT * FROM users");
    CLOG_TRACE("SQLConnector", "Query returned 42 rows");
    
    CLOG_DEBUG("SQLConnector", "Executing query: INSERT INTO logs");
    CLOG_INFO("DatabaseManager", "Database operations completed successfully");
}

void printLogSummary() {
    std::cout << "\n=== Log Summary ===" << std::endl;
    
    // Print recent logs
    auto recentLogs = appLogger.getRecentLogs(5);
    std::cout << "\nLast 5 log entries:" << std::endl;
    for (const auto& entry : recentLogs) {
        std::cout << "  [" << entry.level << "] " << entry.tag 
                  << ": " << entry.message << std::endl;
    }
    
    // Print errors
    auto errors = appLogger.getLogsByLevel("ERROR");
    std::cout << "\nError messages (" << errors.size() << " total):" << std::endl;
    for (const auto& entry : errors) {
        std::cout << "  " << entry.tag << ": " << entry.message << std::endl;
    }
    
    // Print by category
    auto hardwareLogs = appLogger.getLogsByCategory("HARDWARE");
    std::cout << "\nHardware messages (" << hardwareLogs.size() << " total):" << std::endl;
    for (const auto& entry : hardwareLogs) {
        std::cout << "  [" << entry.level << "] " << entry.tag 
                  << ": " << entry.message << std::endl;
    }
}

int main() {
    std::cout << "=== CLog Callback Integration Example ===" << std::endl;
    
    // Set up CLog to use our callback
    clog::Logger::setCallback(clogCallback);
    clog::Logger::setLevel(clog::Level::TRACE);  // Show all messages
    
    CLOG_INFO("Main", "Application starting with CLog integration");
    
    std::cout << "\n--- Simulating Application Subsystems ---" << std::endl;
    
    // Simulate different subsystems
    simulateNetworkSubsystem();
    std::cout << std::endl;
    
    simulateHardwareSubsystem();
    std::cout << std::endl;
    
    simulateDatabaseSubsystem();
    std::cout << std::endl;
    
    CLOG_INFO("Main", "All subsystems initialized");
    
    // Demonstrate log analysis
    printLogSummary();
    
    // Demonstrate disabling console output
    std::cout << "\n--- Disabling Console Output ---" << std::endl;
    appLogger.enableConsole(false);
    
    CLOG_INFO("Main", "This message goes to file but not console");
    CLOG_WARN("Main", "This warning also goes to file only");
    
    // Re-enable console output
    appLogger.enableConsole(true);
    CLOG_INFO("Main", "Console output re-enabled");
    
    CLOG_INFO("Main", "Application shutdown complete");
    
    return 0;
}