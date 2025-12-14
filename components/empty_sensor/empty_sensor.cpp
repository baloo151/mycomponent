#include "esphome/core/log.h"
#include "empty_sensor.h"

#define OOK_EXPECT_PULSE 0
#define OOK_EXPECT_GAP 1

namespace esphome {
namespace empty_sensor {

static const char *TAG = "empty_sensor.sensor";

void IRAM_ATTR EmptySensorStore::gpio_intr(EmptySensorStore *arg)
{
   static unsigned long last;

   arg->pulse = micros() - last;
   last = last + arg->pulse;
   if (arg->pulse > 65535)
   {
      arg->pulse = 65535;
   }
}

void EmptySensor::loop()
{
    // this->publish_state(counter); // Publie la valeur du compteur sur le capteur
}

void EmptySensor::dump_config()
{
    ESP_LOGCONFIG(TAG, "Empty sensor");
    LOG_SENSOR("", "Empty Sensor", this);
    LOG_PIN("  Pin: ", this->pin_);
}

} // namespace empty_sensor
} // namespace esphome