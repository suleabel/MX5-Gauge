#pragma once

#include <Arduino.h>

enum class SensorSource : uint8_t { Analog, Demo };
enum class DemoWaveform : uint8_t { Sine, Triangle, Random, Constant };
enum class ProfileType : uint8_t { Linear, Multipoint, Ntc };

struct CalibrationPoint { float input = 0; float output = 0; };

struct SensorProfile {
    uint16_t id = 0;
    String name;
    ProfileType type = ProfileType::Linear;
    CalibrationPoint points[12];
    uint8_t pointCount = 0;
    float ntcA = 0.001129148;
    float ntcB = 0.000234125;
    float ntcC = 0.0000000876741;
    float pullupOhms = 10000;
};

struct SensorConfig {
    uint16_t id = 0;
    String name;
    String unit;
    SensorSource source = SensorSource::Demo;
    bool enabled = true;
    float minimum = 0;
    float maximum = 100;
    float warningLow = 0;
    float warningHigh = 90;
    uint16_t profileId = 0;
    uint8_t analogPin = 33;
    DemoWaveform waveform = DemoWaveform::Sine;
    float speed = 0.2;
    float constantValue = 50;
    float currentValue = 0;
};
