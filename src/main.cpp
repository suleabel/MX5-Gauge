#include <Arduino.h>
#include <LittleFS.h>

#include "app_wifi_manager.h"
#include "gauge_manager.h"
#include "sensor_manager.h"
#include "web_server_manager.h"

namespace {
AppWiFiManager wifiManager;
SensorManager sensorManager;
GaugeManager gaugeManager;
WebServerManager webServer;

void printStartupSummary() {
    Serial.println();
    Serial.println("================================");
    Serial.println(" MX-5 Smart Gauge");
    Serial.println("================================");
    Serial.printf("Halozat: %s\n", wifiManager.ssid());
    Serial.printf("IP-cim: http://%s\n", wifiManager.ipAddress().toString().c_str());
    Serial.println("================================");
}
}  // namespace

void setup() {
    Serial.begin(115200);
    delay(250);

    if (!LittleFS.begin(true)) {
        Serial.println("HIBA: A LittleFS nem indithato.");
        return;
    }

    if (!wifiManager.begin()) {
        Serial.println("HIBA: A Wi-Fi Access Point nem indithato.");
        return;
    }

    sensorManager.begin();
    gaugeManager.begin();
    webServer.begin(wifiManager, sensorManager, gaugeManager);
    printStartupSummary();
}

void loop() {
    sensorManager.update();
    webServer.handleClient();
    delay(2);
}
