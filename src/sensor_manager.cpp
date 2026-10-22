#include "sensor_manager.h"

#include <Arduino.h>
#include <math.h>

namespace { constexpr float kPi = 3.14159265358979323846f; }

void SensorManager::begin() {
    randomSeed(esp_random());
    SensorProfile oilProfile;
    oilProfile.name = "0.5–4.5 V / 0–10 bar";
    oilProfile.type = ProfileType::Linear;
    oilProfile.points[0] = {0.5f, 0.0f};
    oilProfile.points[1] = {4.5f, 10.0f};
    oilProfile.pointCount = 2;
    upsertProfile(oilProfile);

    SensorConfig pressure;
    pressure.name = "Demo oil pressure";
    pressure.unit = "bar";
    pressure.minimum = 0;
    pressure.maximum = 10;
    pressure.warningHigh = 8;
    pressure.waveform = DemoWaveform::Sine;
    pressure.speed = 0.12f;
    upsertSensor(pressure);

    SensorConfig temperature;
    temperature.name = "Demo oil temperature";
    temperature.unit = "°C";
    temperature.minimum = 20;
    temperature.maximum = 130;
    temperature.warningHigh = 115;
    temperature.waveform = DemoWaveform::Triangle;
    temperature.speed = 0.04f;
    upsertSensor(temperature);
}

void SensorManager::update() {
    const uint32_t now = millis();
    if (now - lastUpdateMs_ < 100) return;
    lastUpdateMs_ = now;
    const float seconds = now / 1000.0f;
    for (uint8_t i = 0; i < sensorCount_; ++i) {
        SensorConfig& item = sensors_[i];
        if (!item.enabled) continue;
        if (item.source == SensorSource::Demo) item.currentValue = generateDemo(item, seconds);
        else item.currentValue = convertAnalog(item, analogReadMilliVolts(item.analogPin) / 1000.0f);
    }
}

SensorConfig* SensorManager::upsertSensor(const SensorConfig& value) {
    SensorConfig* target = value.id ? sensor(value.id) : nullptr;
    if (!target) {
        if (sensorCount_ >= kMaxSensors) return nullptr;
        target = &sensors_[sensorCount_++];
        *target = SensorConfig{};
        target->id = nextSensorId_++;
    }
    const uint16_t id = target->id;
    const float current = target->currentValue;
    *target = value;
    target->id = id;
    target->currentValue = current;
    return target;
}

SensorProfile* SensorManager::upsertProfile(const SensorProfile& value) {
    SensorProfile* target = value.id ? profile(value.id) : nullptr;
    if (!target) {
        if (profileCount_ >= kMaxProfiles) return nullptr;
        target = &profiles_[profileCount_++];
        *target = SensorProfile{};
        target->id = nextProfileId_++;
    }
    const uint16_t id = target->id;
    *target = value;
    target->id = id;
    return target;
}

bool SensorManager::removeSensor(uint16_t id) {
    for (uint8_t i = 0; i < sensorCount_; ++i) if (sensors_[i].id == id) {
        for (uint8_t j = i; j + 1 < sensorCount_; ++j) sensors_[j] = sensors_[j + 1];
        --sensorCount_; return true;
    }
    return false;
}

bool SensorManager::removeProfile(uint16_t id) {
    for (uint8_t i = 0; i < profileCount_; ++i) if (profiles_[i].id == id) {
        for (uint8_t j = i; j + 1 < profileCount_; ++j) profiles_[j] = profiles_[j + 1];
        --profileCount_;
        for (uint8_t j = 0; j < sensorCount_; ++j) if (sensors_[j].profileId == id) sensors_[j].profileId = 0;
        return true;
    }
    return false;
}

SensorConfig* SensorManager::sensor(uint16_t id) { for (auto& item : sensors_) if (item.id == id) return &item; return nullptr; }
SensorProfile* SensorManager::profile(uint16_t id) { for (auto& item : profiles_) if (item.id == id) return &item; return nullptr; }
const SensorConfig& SensorManager::sensorAt(uint8_t i) const { return sensors_[i]; }
const SensorProfile& SensorManager::profileAt(uint8_t i) const { return profiles_[i]; }
uint8_t SensorManager::sensorCount() const { return sensorCount_; }
uint8_t SensorManager::profileCount() const { return profileCount_; }

float SensorManager::generateDemo(SensorConfig& item, float seconds) {
    const float range = item.maximum - item.minimum;
    if (item.waveform == DemoWaveform::Constant) return constrain(item.constantValue, item.minimum, item.maximum);
    if (item.waveform == DemoWaveform::Random) {
        const float step = range * item.speed * 0.05f;
        return constrain(item.currentValue + (random(-1000, 1001) / 1000.0f) * step, item.minimum, item.maximum);
    }
    const float phase = fmodf(seconds * item.speed, 1.0f);
    const float normalized = item.waveform == DemoWaveform::Sine ? (sinf(phase * 2 * kPi) + 1) * 0.5f : (phase < 0.5f ? phase * 2 : 2 - phase * 2);
    return item.minimum + normalized * range;
}

float SensorManager::convertAnalog(const SensorConfig& item, float voltage) const {
    for (uint8_t i = 0; i < profileCount_; ++i) if (profiles_[i].id == item.profileId) return applyProfile(profiles_[i], voltage);
    return voltage;
}

float SensorManager::applyProfile(const SensorProfile& p, float input) const {
    if (p.type == ProfileType::Ntc) {
        if (input <= 0 || input >= 3.3f) return NAN;
        const float resistance = p.pullupOhms * input / (3.3f - input);
        const float logR = logf(resistance);
        return 1.0f / (p.ntcA + p.ntcB * logR + p.ntcC * logR * logR * logR) - 273.15f;
    }
    if (p.pointCount < 2) return input;
    if (input <= p.points[0].input) return p.points[0].output;
    for (uint8_t i = 1; i < p.pointCount; ++i) if (input <= p.points[i].input) {
        const auto& a = p.points[i - 1]; const auto& b = p.points[i];
        return a.output + (input - a.input) * (b.output - a.output) / (b.input - a.input);
    }
    return p.points[p.pointCount - 1].output;
}
