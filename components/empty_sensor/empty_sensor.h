#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace empty_sensor {

class EmptySensor : public sensor::Sensor, public PollingComponent {
    public:
        EmptySensor() : counter(0) {}  // Initialisation du compteur

    void setup() override;
    void loop() override;
    void update() override;
    void dump_config() override;

    private:
        int counter;  // Variable pour le compteur
};

} //namespace empty_sensor
} //namespace esphome