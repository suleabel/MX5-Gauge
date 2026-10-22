#include "app_wifi_manager.h"

bool AppWiFiManager::begin() {
    WiFi.persistent(false);
    WiFi.mode(WIFI_AP);

    const IPAddress address(192, 168, 4, 1);
    const IPAddress gateway(192, 168, 4, 1);
    const IPAddress subnet(255, 255, 255, 0);

    if (!WiFi.softAPConfig(address, gateway, subnet)) {
        return false;
    }

    return WiFi.softAP(kSsid, kPassword, 1, false, 4);
}

const char* AppWiFiManager::ssid() const { return kSsid; }
IPAddress AppWiFiManager::ipAddress() const { return WiFi.softAPIP(); }
uint8_t AppWiFiManager::connectedClients() const { return WiFi.softAPgetStationNum(); }
