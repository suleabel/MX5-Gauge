#include "gauge_manager.h"

void GaugeManager::begin() {
    GaugeConfig oil;
    oil.name = "Olajnyomás";
    oil.type = GaugeType::ArcDigital;
    oil.primarySensorId = 1;
    oil.secondarySensorId = 2;
    oil.slots[0].sensorId = 1;
    oil.slots[1].sensorId = 2;
    GaugeConfig* saved = upsert(oil);
    if (saved) activeId_ = saved->id;

    GaugeConfig temperatures;
    temperatures.name = "Motoradatok";
    temperatures.type = GaugeType::DualDigital;
    temperatures.primarySensorId = 2;
    temperatures.secondarySensorId = 1;
    temperatures.slots[0].sensorId = 2;
    temperatures.slots[0].minimum = 20;
    temperatures.slots[0].maximum = 130;
    temperatures.slots[1].sensorId = 1;
    temperatures.scaleMinimum = 20;
    temperatures.scaleMaximum = 130;
    temperatures.accentColor = "#39c6f4";
    upsert(temperatures);
}

GaugeConfig* GaugeManager::upsert(const GaugeConfig& value) {
    GaugeConfig* target = value.id ? gauge(value.id) : nullptr;
    if (!target) {
        if (count_ >= kMaxGauges) return nullptr;
        target = &gauges_[count_++];
        *target = GaugeConfig{};
        target->id = nextId_++;
    }
    const uint16_t id = target->id;
    *target = value;
    target->id = id;
    if (activeId_ == 0) activeId_ = id;
    return target;
}

bool GaugeManager::remove(uint16_t id) {
    for (uint8_t i = 0; i < count_; ++i) if (gauges_[i].id == id) {
        for (uint8_t j = i; j + 1 < count_; ++j) gauges_[j] = gauges_[j + 1];
        --count_;
        if (activeId_ == id) activeId_ = count_ ? gauges_[0].id : 0;
        return true;
    }
    return false;
}

bool GaugeManager::move(uint16_t id, int8_t direction) {
    for (uint8_t i = 0; i < count_; ++i) if (gauges_[i].id == id) {
        const int target = static_cast<int>(i) + direction;
        if (target < 0 || target >= count_) return false;
        GaugeConfig temp = gauges_[i]; gauges_[i] = gauges_[target]; gauges_[target] = temp;
        return true;
    }
    return false;
}

bool GaugeManager::setActive(uint16_t id) { if (!gauge(id)) return false; activeId_ = id; return true; }
GaugeConfig* GaugeManager::gauge(uint16_t id) { for (uint8_t i=0;i<count_;++i) if(gauges_[i].id==id) return &gauges_[i]; return nullptr; }
const GaugeConfig& GaugeManager::gaugeAt(uint8_t index) const { return gauges_[index]; }
uint8_t GaugeManager::count() const { return count_; }
uint16_t GaugeManager::activeId() const { return activeId_; }
