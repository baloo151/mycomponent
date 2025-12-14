#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace empty_sensor {

<<<<<<< Updated upstream
class EmptySensor : public sensor::Sensor, public PollingComponent {
=======
class EmptySensor : public sensor::Sensor, public Component {
>>>>>>> Stashed changes
    public:
        EmptySensor() : counter(0) {}  // Initialisation du compteur

    void setup() override;
    void loop() override;
    void dump_config() override;

    private:
        int counter;  // Variable pour le compteur
};

} //namespace empty_sensor
} //namespace esphome