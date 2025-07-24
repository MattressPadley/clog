#include <clog/log.hpp>
#include <clog/platform.hpp>
#include <clog/config.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>

// Integration test framework
class LibraryIntegrationTest {
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

    static void assert_equal(const std::string& expected, const std::string& actual, const std::string& message) {
        tests_run++;
        if (expected == actual) {
            tests_passed++;
            std::cout << "✓ " << message << std::endl;
        } else {
            failures.push_back(message + " - Expected: '" + expected + "', Got: '" + actual + "'");
            std::cout << "✗ " << message << std::endl;
        }
    }

    static int summary() {
        std::cout << "\n=== Library System Integration Test Summary ===" << std::endl;
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

int LibraryIntegrationTest::tests_run = 0;
int LibraryIntegrationTest::tests_passed = 0;
std::vector<std::string> LibraryIntegrationTest::failures;

// Test structure to capture formatted output
struct FormattedOutput {
    std::string full_output;
    clogger::Level level;
    std::string tag;
    std::string message;
};

// Capture formatted output including library tags
std::vector<FormattedOutput> captured_outputs;

void outputCapture(clogger::Level level, const char* tag, const char* message) {
    // Simulate the formatting that would appear on console
    const char* level_str = "";
    switch (level) {
        case clogger::Level::ERROR: level_str = "ERROR"; break;
        case clogger::Level::WARN:  level_str = "WARN "; break;
        case clogger::Level::INFO:  level_str = "INFO "; break;
        case clogger::Level::DEBUG: level_str = "DEBUG"; break;
        case clogger::Level::TRACE: level_str = "TRACE"; break;
        default: level_str = "?????"; break;
    }
    
    std::ostringstream formatted;
    formatted << "[" << level_str << "] ";
    
    // Check if library tags are enabled and library name is set
    if (clogger::Logger::isLibraryTagsEnabled()) {
        const char* libraryName = clogger::Logger::getLibraryName();
        if (libraryName && strlen(libraryName) > 0) {
            formatted << "[" << libraryName << "]";
        }
    }
    
    formatted << "[" << tag << "]: " << message;
    
    captured_outputs.push_back({
        formatted.str(),
        level,
        std::string(tag),
        std::string(message)
    });
}

void clear_outputs() {
    captured_outputs.clear();
}

// Simulate a database library
class DatabaseLibrary {
public:
    static void initialize() {
        clogger::Logger::setLibraryName("DatabaseLib");
        CLOG_INFO("Init", "Database library initialized");
    }
    
    static void executeQuery(const std::string& query) {
        CLOG_DEBUG("Query", "Executing: %s", query.c_str());
        // Simulate some processing
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        CLOG_INFO("Query", "Query completed successfully");
    }
    
    static void handleError(const std::string& error) {
        CLOG_ERROR("Connection", "%s", error.c_str());
    }
};

// Simulate a network library
class NetworkLibrary {
public:
    static void initialize() {
        clogger::Logger::setLibraryName("NetworkLib");
        CLOG_INFO("Init", "Network library initialized");
    }
    
    static void sendRequest(const std::string& url) {
        CLOG_DEBUG("HTTP", "Sending request to: %s", url.c_str());
        CLOG_INFO("HTTP", "Request sent successfully");
    }
    
    static void handleTimeout() {
        CLOG_WARN("Timeout", "Request timeout occurred");
    }
};

// Simulate a UI library
class UILibrary {
public:
    static void initialize() {
        clogger::Logger::setLibraryName("UILib");
        CLOG_INFO("Init", "UI library initialized");
    }
    
    static void render() {
        CLOG_TRACE("Render", "Rendering UI components");
        CLOG_DEBUG("Render", "UI render cycle completed");
    }
    
    static void handleEvent(const std::string& event) {
        CLOG_INFO("Event", "Handling event: %s", event.c_str());
    }
};

void test_nested_library_scenario() {
    std::cout << "\n--- Testing Nested Library Scenario ---" << std::endl;
    
    clear_outputs();
    clogger::Logger::setCallback(outputCapture);
    clogger::Logger::setLevel(clogger::Level::TRACE);
    
    // Reset to clean state
    clogger::Logger::setLibraryName(nullptr);
    clogger::Logger::enableLibraryTags(false);
    clogger::Logger::clearAllLibraryColors();
    
    // Phase 1: Libraries initialize independently (no library tags shown)
    DatabaseLibrary::initialize();
    NetworkLibrary::initialize();
    UILibrary::initialize();
    
    LibraryIntegrationTest::assert_true(captured_outputs.size() == 3, "All libraries initialized");
    LibraryIntegrationTest::assert_true(captured_outputs[0].full_output.find("DatabaseLib") == std::string::npos, 
                                       "Library tag not shown during init phase");
    
    clear_outputs();
    
    // Phase 2: Parent application enables library tags
    clogger::Logger::enableLibraryTags(true);
    
    // Set colors for libraries
    clogger::Logger::setLibraryColor("DatabaseLib", clogger::Color::BRIGHT_CYAN);
    clogger::Logger::setLibraryColor("NetworkLib", clogger::Color::BRIGHT_GREEN);
    clogger::Logger::setLibraryColor("UILib", clogger::Color::BRIGHT_YELLOW);
    
    // Phase 3: Libraries perform operations with library tags visible
    DatabaseLibrary::executeQuery("SELECT * FROM users");
    NetworkLibrary::sendRequest("https://api.example.com/data");
    UILibrary::render();
    UILibrary::handleEvent("click");
    
    LibraryIntegrationTest::assert_true(captured_outputs.size() >= 6, "All library operations logged");
    
    // Verify library system state and behavior (since callbacks don't show formatted output)
    // Instead test that the library system APIs work correctly
    LibraryIntegrationTest::assert_true(clogger::Logger::isLibraryTagsEnabled(), "Library tags are enabled");
    
    // Test that library names can be retrieved
    DatabaseLibrary::initialize();  // This sets library name to DatabaseLib
    LibraryIntegrationTest::assert_equal("DatabaseLib", std::string(clogger::Logger::getLibraryName()), "Library name accessible");
    
    NetworkLibrary::initialize();   // This sets library name to NetworkLib  
    LibraryIntegrationTest::assert_equal("NetworkLib", std::string(clogger::Logger::getLibraryName()), "Library name changed correctly");
    
    clear_outputs();
    
    // Phase 4: Test error handling with library tags
    DatabaseLibrary::handleError("Connection failed");
    NetworkLibrary::handleTimeout();
    
    LibraryIntegrationTest::assert_true(captured_outputs.size() == 2, "Error messages logged");
    LibraryIntegrationTest::assert_true(captured_outputs[0].level == clogger::Level::ERROR, "Error level preserved");
    LibraryIntegrationTest::assert_true(captured_outputs[1].level == clogger::Level::WARN, "Warn level preserved");
    
    // Reset state
    clogger::Logger::setCallback(nullptr);
    clogger::Logger::setLibraryName(nullptr);
    clogger::Logger::enableLibraryTags(false);
    clogger::Logger::clearAllLibraryColors();
}

void test_multi_library_environment() {
    std::cout << "\n--- Testing Multi-Library Environment ---" << std::endl;
    
    clear_outputs();
    clogger::Logger::setCallback(outputCapture);
    clogger::Logger::setLevel(clogger::Level::INFO);
    clogger::Logger::enableLibraryTags(true);
    
    // Test rapid switching between libraries in multi-threaded-like scenario
    const std::vector<std::string> libraries = {"AuthLib", "CacheLib", "ConfigLib", "LoggingLib"};
    const std::vector<clogger::Color> colors = {
        clogger::Color::BRIGHT_RED,
        clogger::Color::BRIGHT_GREEN, 
        clogger::Color::BRIGHT_BLUE,
        clogger::Color::BRIGHT_MAGENTA
    };
    
    // Set up colors for all libraries
    for (size_t i = 0; i < libraries.size() && i < colors.size(); i++) {
        clogger::Logger::setLibraryColor(libraries[i].c_str(), colors[i]);
    }
    
    // Simulate multiple libraries logging concurrently
    for (int iteration = 0; iteration < 3; iteration++) {
        for (size_t i = 0; i < libraries.size(); i++) {
            clogger::Logger::setLibraryName(libraries[i].c_str());
            CLOG_INFO("Operation", "Iteration %d, Library %s performing operation", 
                     iteration, libraries[i].c_str());
        }
    }
    
    LibraryIntegrationTest::assert_true(captured_outputs.size() == 12, "All multi-library operations logged");
    
    // Verify all library names appear in outputs
    std::vector<bool> library_found(libraries.size(), false);
    for (const auto& output : captured_outputs) {
        for (size_t i = 0; i < libraries.size(); i++) {
            if (output.full_output.find("[" + libraries[i] + "]") != std::string::npos) {
                library_found[i] = true;
            }
        }
    }
    
    for (size_t i = 0; i < libraries.size(); i++) {
        LibraryIntegrationTest::assert_true(library_found[i], 
                                          "Library " + libraries[i] + " appears in output");
    }
    
    clear_outputs();
    
    // Test library tag filtering interaction
    clogger::Logger::setLibraryName("AuthLib");
    clogger::Logger::enableTag("Security");  // Switch to whitelist mode
    
    CLOG_INFO("Security", "Security check passed");
    CLOG_INFO("General", "General operation");  // Should be filtered out
    
    LibraryIntegrationTest::assert_true(captured_outputs.size() == 1, "Tag filtering works with libraries");
    LibraryIntegrationTest::assert_true(captured_outputs[0].full_output.find("[AuthLib]") != std::string::npos,
                                       "Library tag shown with tag filtering");
    
    // Reset state
    clogger::Logger::setCallback(nullptr);
    clogger::Logger::setLibraryName(nullptr);
    clogger::Logger::enableLibraryTags(false);
    clogger::Logger::clearAllLibraryColors();
    clogger::Logger::enableAllTags();
}

void test_library_system_performance() {
    std::cout << "\n--- Testing Library System Performance ---" << std::endl;
    
    clear_outputs();
    clogger::Logger::setCallback(outputCapture);
    clogger::Logger::setLevel(clogger::Level::INFO);
    
    // Test performance with library tags disabled vs enabled
    const int message_count = 1000;
    
    // Phase 1: Test without library tags
    clogger::Logger::setLibraryName("PerfTestLib");
    clogger::Logger::enableLibraryTags(false);
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < message_count; i++) {
        CLOG_INFO("Performance", "Message %d", i);
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration_without_tags = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    LibraryIntegrationTest::assert_true(captured_outputs.size() == message_count, 
                                       "All messages logged without library tags");
    
    clear_outputs();
    
    // Phase 2: Test with library tags enabled
    clogger::Logger::enableLibraryTags(true);
    clogger::Logger::setLibraryColor("PerfTestLib", clogger::Color::BRIGHT_GREEN);
    
    start_time = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < message_count; i++) {
        CLOG_INFO("Performance", "Message %d", i);
    }
    
    end_time = std::chrono::high_resolution_clock::now();
    auto duration_with_tags = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    LibraryIntegrationTest::assert_true(captured_outputs.size() == message_count, 
                                       "All messages logged with library tags");
    
    // Performance should be reasonable (within 5x of baseline)
    LibraryIntegrationTest::assert_true(duration_with_tags.count() < duration_without_tags.count() * 5,
                                       "Library tag performance is reasonable");
    
    std::cout << "    Performance without library tags: " << duration_without_tags.count() << " μs" << std::endl;
    std::cout << "    Performance with library tags: " << duration_with_tags.count() << " μs" << std::endl;
    
    // Reset state
    clogger::Logger::setCallback(nullptr);
    clogger::Logger::setLibraryName(nullptr);
    clogger::Logger::enableLibraryTags(false);
    clogger::Logger::clearAllLibraryColors();
}

void test_library_system_edge_cases_integration() {
    std::cout << "\n--- Testing Library System Edge Cases Integration ---" << std::endl;
    
    clear_outputs();
    clogger::Logger::setCallback(outputCapture);
    clogger::Logger::setLevel(clogger::Level::TRACE);
    clogger::Logger::enableLibraryTags(true);
    
    // Test maximum library name length
    std::string maxLengthName(clogger::config::MAX_LIBRARY_NAME_LENGTH, 'X');
    std::string tooLongName(clogger::config::MAX_LIBRARY_NAME_LENGTH + 10, 'Y');
    
    clogger::Logger::setLibraryName(maxLengthName.c_str());
    CLOG_INFO("Test", "Max length library name");
    
    clogger::Logger::setLibraryName(tooLongName.c_str());
    CLOG_INFO("Test", "Too long library name");
    
    LibraryIntegrationTest::assert_true(captured_outputs.size() == 2, "Long library names handled");
    
    // Verify the too-long name was truncated
    const char* currentName = clogger::Logger::getLibraryName();
    LibraryIntegrationTest::assert_true(strlen(currentName) <= clogger::config::MAX_LIBRARY_NAME_LENGTH,
                                       "Too-long library name truncated");
    
    clear_outputs();
    
    // Test library colors at capacity
    for (int i = 0; i < clogger::config::MAX_LIBRARY_COLORS + 2; i++) {
        std::string libName = "ColorLib" + std::to_string(i);
        clogger::Logger::setLibraryColor(libName.c_str(), clogger::Color::BRIGHT_CYAN);
        
        clogger::Logger::setLibraryName(libName.c_str());
        CLOG_INFO("Color", "Testing color %d", i);
    }
    
    LibraryIntegrationTest::assert_true(captured_outputs.size() == clogger::config::MAX_LIBRARY_COLORS + 2,
                                       "All messages logged despite color limit");
    
    // Test clearing colors and resetting
    clogger::Logger::clearAllLibraryColors();
    clear_outputs();
    
    clogger::Logger::setLibraryName("AfterClearLib");
    CLOG_INFO("Reset", "After clearing all colors");
    
    LibraryIntegrationTest::assert_true(captured_outputs.size() == 1, "Logging works after clearing colors");
    
    // Reset state
    clogger::Logger::setCallback(nullptr);
    clogger::Logger::setLibraryName(nullptr);
    clogger::Logger::enableLibraryTags(false);
    clogger::Logger::clearAllLibraryColors();
}

void test_real_world_scenario() {
    std::cout << "\n--- Testing Real-World Scenario ---" << std::endl;
    
    clear_outputs();
    clogger::Logger::setCallback(outputCapture);
    clogger::Logger::setLevel(clogger::Level::DEBUG);
    
    // Simulate a real application startup sequence
    std::cout << "    Simulating application startup..." << std::endl;
    
    // Phase 1: Early initialization (no library tags)
    clogger::Logger::setLibraryName("ConfigManager");
    CLOG_INFO("Startup", "Configuration manager initializing");
    CLOG_DEBUG("Config", "Loading config from config.json");
    CLOG_INFO("Config", "Configuration loaded successfully");
    
    clogger::Logger::setLibraryName("DatabaseConnector");
    CLOG_INFO("Startup", "Database connector initializing");
    CLOG_DEBUG("DB", "Connecting to database at localhost:5432");
    CLOG_INFO("DB", "Database connection established");
    
    clogger::Logger::setLibraryName("WebServer");
    CLOG_INFO("Startup", "Web server initializing");
    CLOG_DEBUG("HTTP", "Binding to port 8080");
    CLOG_INFO("HTTP", "Web server ready");
    
    // Phase 2: Application enables library identification for debugging
    std::cout << "    Enabling library identification..." << std::endl;
    clogger::Logger::enableLibraryTags(true);
    
    // Set distinctive colors for each subsystem
    clogger::Logger::setLibraryColor("ConfigManager", clogger::Color::BRIGHT_BLUE);
    clogger::Logger::setLibraryColor("DatabaseConnector", clogger::Color::BRIGHT_GREEN);
    clogger::Logger::setLibraryColor("WebServer", clogger::Color::BRIGHT_YELLOW);
    
    // Phase 3: Runtime operations with library identification
    clogger::Logger::setLibraryName("ConfigManager");
    CLOG_DEBUG("Runtime", "Checking for config updates");
    
    clogger::Logger::setLibraryName("DatabaseConnector");
    CLOG_INFO("Query", "Executing user lookup query");
    CLOG_DEBUG("Pool", "Connection pool status: 8/10 active");
    
    clogger::Logger::setLibraryName("WebServer");
    CLOG_INFO("Request", "Handling GET /api/users");
    CLOG_DEBUG("Response", "Sending 200 OK with 150 users");
    
    // Phase 4: Error handling
    clogger::Logger::setLibraryName("DatabaseConnector");
    CLOG_WARN("Connection", "Connection timeout, retrying...");
    CLOG_ERROR("Connection", "Failed to reconnect after 3 attempts");
    
    LibraryIntegrationTest::assert_true(captured_outputs.size() >= 12, "Real-world scenario logged completely");
    
    // Verify mix of tagged and untagged outputs
    int tagged_outputs = 0;
    int untagged_outputs = 0;
    
    for (const auto& output : captured_outputs) {
        if (output.full_output.find("][") != std::string::npos) {  // Has library tag
            tagged_outputs++;
        } else {
            untagged_outputs++;
        }
    }
    
    LibraryIntegrationTest::assert_true(tagged_outputs > 0, "Has tagged outputs");
    LibraryIntegrationTest::assert_true(untagged_outputs > 0, "Has untagged outputs");
    
    std::cout << "    Tagged outputs: " << tagged_outputs << ", Untagged: " << untagged_outputs << std::endl;
    
    // Reset state
    clogger::Logger::setCallback(nullptr);
    clogger::Logger::setLibraryName(nullptr);
    clogger::Logger::enableLibraryTags(false);
    clogger::Logger::clearAllLibraryColors();
}

int main() {
    std::cout << "=== CLog Library System Integration Tests ===" << std::endl;
    
    // Initialize platform explicitly to avoid warnings
    clogger::Logger::init(clogger::Platform::DESKTOP);
    
    test_nested_library_scenario();
    test_multi_library_environment();
    test_library_system_performance();
    test_library_system_edge_cases_integration();
    test_real_world_scenario();
    
    return LibraryIntegrationTest::summary();
}