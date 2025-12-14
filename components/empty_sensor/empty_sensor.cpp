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

void detect_pulse_length(unsigned int *timings, unsigned int count)
{
    unsigned int pulse_length;
    float f_pulse_length;
    unsigned int pulse_length_min;
    unsigned int pulse_length_max;
    int pulses_count = 0;

    unsigned int i = count - 2;
    pulse_length = pulse_length_min = pulse_length_max = timings[i];
    f_pulse_length = pulse_length;

    while ((i >= 0) && (timings[i] >= pulse_length*0.8) && (timings[i] <= pulse_length*1.2)) {
        f_pulse_length = (f_pulse_length * pulses_count + timings[i]) / (pulses_count + 1);
        pulse_length = int(f_pulse_length);
        pulses_count++;

        if (timings[i] < pulse_length_min)
        {
            pulse_length_min = timings[i];
        }

        if (timings[i] > pulse_length_max)
        {
            pulse_length_max = timings[i];
        }

        i = i - 2;
    }

    ESP_LOGD(TAG, "Count: %d Mean: %d Min: %d Max: %d", pulses_count, pulse_length, pulse_length_min, pulse_length_max);
}

void EmptySensor::loop()
{
    // this->publish_state(counter); // Publie la valeur du compteur sur le capteur

   static unsigned int idx = 0;
   unsigned int i;

   cli();

    if (this->store_.pulse > 0)
    {
        if (this->store_.pulse < 200)
        {
            // noise
            idx = 0;
        }
        else
        {
            this->store_.timings_data[idx] = this->store_.pulse;
            idx++;
            if (idx >= BUFFSIZE)
            {
                idx = 0;
            }


            if ((this->store_.pulse > 15000) && (idx >= 32))
            {
                detect_pulse_length(this->store_.timings_data, idx);
                // decode_ook_ppm_nexus(this->store_.timings_data, idx);

                idx = 0;
            }
        }

        this->store_.pulse = 0;
    }

   sei();
}

void EmptySensor::dump_config()
{
    LOG_SENSOR("", "Empty Sensor", this);
    LOG_PIN("  Pin: ", this->pin_);
}

} // namespace empty_sensor
} // namespace esphome