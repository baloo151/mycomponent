#include "esphome/core/log.h"
#include "empty_sensor.h"

namespace esphome {
namespace empty_sensor {

static const char *TAG = "empty_sensor.sensor";

void EmptySensor::setup() {
    // Afficher l'intervalle de mise à jour dans les logs
    ESP_LOGD(TAG, "Update interval: %d ms", this->get_update_interval());  
}

void EmptySensor::loop() {
    // Incrémenter le compteur toutes les secondes
    counter++;  
}

void EmptySensor::update() {
    // Publier la valeur du compteur
    this->publish_state(counter);  // Publie la valeur du compteur sur le capteur
}

void EmptySensor::dump_config() {
    ESP_LOGCONFIG(TAG, "Empty sensor");
}

} //namespace empty_sensor
} //namespace esphome