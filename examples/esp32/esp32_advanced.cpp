#include <clog/log.hpp>
#include <WiFi.h>
#include <WebServer.h>

// WiFi credentials (replace with your own)
const char* ssid = "YourWiFiNetwork";
const char* password = "YourWiFiPassword";

// Web server
WebServer server(80);

// Task handles for FreeRTOS
TaskHandle_t sensorTaskHandle = NULL;
TaskHandle_t networkTaskHandle = NULL;

// Global variables
bool wifiConnected = false;
float temperature = 25.0;
int humidity = 60;

void setup() {
    Serial.begin(115200);
    
    // Initialize CLog with callback for structured logging
    clog::Logger::setCallback(esp32LogCallback);
    clog::Logger::setLevel(clog::Level::DEBUG);
    
    CLOG_INFO("System", "ESP32 CLog Advanced Example starting...");
    CLOG_DEBUG("System", "Free heap: %d bytes", ESP.getFreeHeap());
    
    // Initialize WiFi
    initWiFi();
    
    // Initialize web server
    initWebServer();
    
    // Create FreeRTOS tasks
    createTasks();
    
    CLOG_INFO("System", "Initialization complete");
}

void loop() {
    // Handle web server requests
    server.handleClient();
    
    // Small delay
    delay(10);
}

// Custom log callback for ESP32
void esp32LogCallback(clog::Level level, const char* tag, const char* message) {
    // Add timestamp and task info
    unsigned long timestamp = millis();
    const char* taskName = pcTaskGetTaskName(NULL);
    
    const char* levelStr = "";
    const char* color = "";
    
    switch (level) {
        case clog::Level::ERROR: 
            levelStr = "ERROR"; 
            color = "\033[91m";  // Red
            break;
        case clog::Level::WARN:  
            levelStr = "WARN "; 
            color = "\033[93m";  // Yellow
            break;
        case clog::Level::INFO:  
            levelStr = "INFO "; 
            color = "\033[92m";  // Green
            break;
        case clog::Level::DEBUG: 
            levelStr = "DEBUG"; 
            color = "\033[94m";  // Blue
            break;
        case clog::Level::TRACE: 
            levelStr = "TRACE"; 
            color = "\033[90m";  // Gray
            break;
    }
    
    Serial.printf("%s[%lu][%s][%s] %s: %s\033[0m\n", 
                  color, timestamp, taskName, levelStr, tag, message);
}

void initWiFi() {
    CLOG_INFO("WiFi", "Connecting to %s", ssid);
    
    WiFi.begin(ssid, password);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        CLOG_DEBUG("WiFi", "Connection attempt %d", attempts + 1);
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        wifiConnected = true;
        CLOG_INFO("WiFi", "Connected! IP: %s", WiFi.localIP().toString().c_str());
        CLOG_DEBUG("WiFi", "RSSI: %d dBm", WiFi.RSSI());
    } else {
        wifiConnected = false;
        CLOG_ERROR("WiFi", "Failed to connect after %d attempts", attempts);
    }
}

void initWebServer() {
    // Root page
    server.on("/", handleRoot);
    
    // API endpoints
    server.on("/api/status", handleStatus);
    server.on("/api/logs", handleLogs);
    
    server.begin();
    CLOG_INFO("WebServer", "Server started on port 80");
}

void createTasks() {
    // Create sensor reading task
    xTaskCreatePinnedToCore(
        sensorTask,
        "SensorTask",
        4096,
        NULL,
        1,
        &sensorTaskHandle,
        0
    );
    
    // Create network monitoring task
    xTaskCreatePinnedToCore(
        networkTask,
        "NetworkTask", 
        4096,
        NULL,
        1,
        &networkTaskHandle,
        1
    );
    
    CLOG_INFO("Tasks", "FreeRTOS tasks created successfully");
}

void sensorTask(void* parameter) {
    CLOG_INFO("SensorTask", "Task started");
    
    while (true) {
        // Simulate temperature reading
        temperature = 20.0 + (random(0, 200) / 10.0);  // 20-40°C
        
        // Simulate humidity reading
        humidity = 40 + random(0, 40);  // 40-80%
        
        CLOG_DEBUG("Sensor", "Temperature: %.1f°C, Humidity: %d%%", temperature, humidity);
        
        // Check for warning conditions
        if (temperature > 35.0) {
            CLOG_WARN("Sensor", "High temperature: %.1f°C", temperature);
        }
        
        if (humidity > 75) {
            CLOG_WARN("Sensor", "High humidity: %d%%", humidity);
        }
        
        // Log memory usage periodically
        static int memoryCounter = 0;
        if (++memoryCounter >= 10) {  // Every 10 readings
            memoryCounter = 0;
            size_t freeHeap = ESP.getFreeHeap();
            CLOG_DEBUG("Memory", "Free heap: %d bytes, task stack: %d", 
                      freeHeap, uxTaskGetStackHighWaterMark(NULL));
        }
        
        vTaskDelay(pdMS_TO_TICKS(2000));  // 2 second delay
    }
}

void networkTask(void* parameter) {
    CLOG_INFO("NetworkTask", "Task started");
    
    while (true) {
        // Monitor WiFi connection
        if (WiFi.status() != WL_CONNECTED) {
            if (wifiConnected) {
                CLOG_ERROR("WiFi", "Connection lost, attempting reconnect");
                wifiConnected = false;
                WiFi.reconnect();
            }
        } else {
            if (!wifiConnected) {
                CLOG_INFO("WiFi", "Connection restored! IP: %s", WiFi.localIP().toString().c_str());
                wifiConnected = true;
            }
            
            // Log signal strength periodically
            static int rssiCounter = 0;
            if (++rssiCounter >= 30) {  // Every 30 * 5 = 150 seconds
                rssiCounter = 0;
                int rssi = WiFi.RSSI();
                if (rssi < -70) {
                    CLOG_WARN("WiFi", "Weak signal: %d dBm", rssi);
                } else {
                    CLOG_DEBUG("WiFi", "Signal strength: %d dBm", rssi);
                }
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(5000));  // 5 second delay
    }
}

// Web server handlers
void handleRoot() {
    String html = "<html><body>";
    html += "<h1>ESP32 CLog Example</h1>";
    html += "<p>Temperature: " + String(temperature) + "°C</p>";
    html += "<p>Humidity: " + String(humidity) + "%</p>";
    html += "<p>WiFi: " + String(wifiConnected ? "Connected" : "Disconnected") + "</p>";
    html += "<p>Free Heap: " + String(ESP.getFreeHeap()) + " bytes</p>";
    html += "<p><a href='/api/status'>Status API</a></p>";
    html += "<p><a href='/api/logs'>Logs API</a></p>";
    html += "</body></html>";
    
    server.send(200, "text/html", html);
    CLOG_DEBUG("WebServer", "Root page served to client");
}

void handleStatus() {
    String json = "{";
    json += "\"temperature\":" + String(temperature) + ",";
    json += "\"humidity\":" + String(humidity) + ",";
    json += "\"wifi_connected\":" + String(wifiConnected ? "true" : "false") + ",";
    json += "\"free_heap\":" + String(ESP.getFreeHeap()) + ",";
    json += "\"uptime\":" + String(millis());
    json += "}";
    
    server.send(200, "application/json", json);
    CLOG_DEBUG("WebServer", "Status API served to client");
}

void handleLogs() {
    // In a real application, you might store recent logs in a buffer
    String response = "Recent log entries would be served here\n";
    response += "This is a placeholder for demonstration\n";
    
    server.send(200, "text/plain", response);
    CLOG_DEBUG("WebServer", "Logs API served to client");
}