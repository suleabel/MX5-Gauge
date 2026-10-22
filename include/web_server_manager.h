#pragma once

#include <WebServer.h>
#include "app_wifi_manager.h"
#include "gauge_manager.h"
#include "sensor_manager.h"

class WebServerManager {
public:
    WebServerManager();
    void begin(const AppWiFiManager& wifiManager, SensorManager& sensorManager, GaugeManager& gaugeManager);
    void handleClient();

private:
    void sendStatus();
    void sendSensors();
    void saveSensor();
    void sendProfiles();
    void saveProfile();
    void sendReadings();
    void sendGauges();
    void saveGauge();
    void sendNotFound();

    WebServer server_;
    const AppWiFiManager* wifiManager_ = nullptr;
    SensorManager* sensorManager_ = nullptr;
    GaugeManager* gaugeManager_ = nullptr;
};
