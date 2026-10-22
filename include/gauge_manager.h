#pragma once

#include <Arduino.h>

enum class GaugeType : uint8_t { ArcDigital, AnalogNeedle, DualDigital, MinimalDigital, DualArc, QuadDigital, DualAnalog };

struct GaugeSensorStyle {
    uint16_t sensorId = 0;
    float minimum = 0;
    float maximum = 10;
    int16_t startAngle = -90;
    bool clockwise = true;
    uint16_t nameScale = 100;
    int16_t nameOffsetX = 0, nameOffsetY = 0;
    uint16_t valueScale = 100;
    int16_t valueOffsetX = 0, valueOffsetY = 0;
    uint16_t unitScale = 100;
    int16_t unitOffsetX = 0, unitOffsetY = 0;
};

struct GaugeConfig {
    uint16_t id = 0;
    String name;
    GaugeType type = GaugeType::ArcDigital;
    uint16_t primarySensorId = 0;
    uint16_t secondarySensorId = 0;
    uint16_t tertiarySensorId = 0;
    uint16_t quaternarySensorId = 0;
    String backgroundColor = "#090d12";
    String accentColor = "#f04438";
    String textColor = "#f5f7fa";
    String warningColor = "#ffb020";
    float scaleMinimum = 0;
    float scaleMaximum = 10;
    bool showUnit = true;
    uint16_t nameScale = 100;
    int16_t nameOffsetX = 0;
    int16_t nameOffsetY = 0;
    uint16_t valueScale = 100;
    int16_t valueOffsetX = 0;
    int16_t valueOffsetY = 0;
    uint16_t unitScale = 100;
    int16_t unitOffsetX = 0;
    int16_t unitOffsetY = 0;
    GaugeSensorStyle slots[4];
};

class GaugeManager {
public:
    static constexpr uint8_t kMaxGauges = 8;
    void begin();
    GaugeConfig* upsert(const GaugeConfig& value);
    bool remove(uint16_t id);
    bool move(uint16_t id, int8_t direction);
    bool setActive(uint16_t id);
    GaugeConfig* gauge(uint16_t id);
    const GaugeConfig& gaugeAt(uint8_t index) const;
    uint8_t count() const;
    uint16_t activeId() const;

private:
    GaugeConfig gauges_[kMaxGauges];
    uint8_t count_ = 0;
    uint16_t nextId_ = 1;
    uint16_t activeId_ = 0;
};
