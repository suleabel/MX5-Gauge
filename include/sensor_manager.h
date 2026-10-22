#pragma once

#include "sensor_types.h"

class SensorManager {
public:
    static constexpr uint8_t kMaxSensors = 16;
    static constexpr uint8_t kMaxProfiles = 8;

    void begin();
    void update();
    SensorConfig* upsertSensor(const SensorConfig& value);
    SensorProfile* upsertProfile(const SensorProfile& value);
    bool removeSensor(uint16_t id);
    bool removeProfile(uint16_t id);
    SensorConfig* sensor(uint16_t id);
    SensorProfile* profile(uint16_t id);
    const SensorConfig& sensorAt(uint8_t index) const;
    const SensorProfile& profileAt(uint8_t index) const;
    uint8_t sensorCount() const;
    uint8_t profileCount() const;

private:
    float generateDemo(SensorConfig& sensor, float seconds);
    float convertAnalog(const SensorConfig& sensor, float voltage) const;
    float applyProfile(const SensorProfile& profile, float input) const;

    SensorConfig sensors_[kMaxSensors];
    SensorProfile profiles_[kMaxProfiles];
    uint8_t sensorCount_ = 0;
    uint8_t profileCount_ = 0;
    uint16_t nextSensorId_ = 1;
    uint16_t nextProfileId_ = 1;
    uint32_t lastUpdateMs_ = 0;
};
