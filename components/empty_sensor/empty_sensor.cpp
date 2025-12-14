#include "esphome/core/log.h"
#include "empty_sensor.h"

namespace esphome
{
namespace empty_sensor
{

static const char *TAG = "empty_sensor.sensor";

void EmptySensor::setup()
{

}

void EmptySensor::loop()
{
    static unsigned long last_time = 0;    // Variable pour mémoriser le dernier temps
    unsigned long current_time = millis(); // Temps actuel en millisecondes

    // Vérifier si une seconde s'est écoulée
    if (current_time - last_time >= 10000)
    {                             // Si 10000 ms (10 seconde) se sont écoulées
        counter += 10;            // Incrémenter le compteur
        last_time = current_time; // Mettre à jour le dernier temps avec le temps actuel

        this->publish_state(counter); // Publie la valeur du compteur sur le capteur
    }
}

void EmptySensor::dump_config()
{
    ESP_LOGCONFIG(TAG, "Empty sensor");
}

} // namespace empty_sensor
} // namespace esphome