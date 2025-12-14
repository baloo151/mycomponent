#include "esphome/core/log.h"
#include "empty_sensor.h"

namespace esphome {
namespace empty_sensor {

static const char *TAG = "empty_sensor.sensor";

void EmptySensor::setup() {
}

void EmptySensor::loop() {
    // Incrémenter le compteur toutes les secondes
    counter++;  
}

void EmptySensor::update() {
    // Publier la valeur du compteur
    ESP_LOGD(TAG, "in update"); 
    this->publish_state(counter);  // Publie la valeur du compteur sur le capteur
}

void EmptySensor::dump_config() {
    ESP_LOGCONFIG(TAG, "Empty sensor");
    ESP_LOGCONFIG(TAG, "Update interval: %d ms", this->get_update_interval());  
}

} //namespace empty_sensor
} //namespace esphome