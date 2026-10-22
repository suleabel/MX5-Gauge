#pragma once

#include <Arduino.h>
#include <WiFi.h>

class AppWiFiManager {
public:
    bool begin();
    const char* ssid() const;
    IPAddress ipAddress() const;
    uint8_t connectedClients() const;

private:
    static constexpr const char* kSsid = "MX5-Gauge";
    static constexpr const char* kPassword = "mx5gauge";
};
