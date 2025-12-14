#include "esphome/core/log.h"
#include "empty_sensor.h"

#define OOK_EXPECT_PULSE 0
#define OOK_EXPECT_GAP 1

namespace esphome {
namespace empty_sensor {

static const char *TAG = "empty_sensor.sensor";

void IRAM_ATTR EmptySensorStore::gpio_intr(EmptySensorStore *arg)
{
    static unsigned long edgeTimeStamp[3] = {0, };
    static bool skip = false;

    // Filter out too short pulses. This method works as a low pass filter.
    edgeTimeStamp[1] = edgeTimeStamp[2];
    edgeTimeStamp[2] = micros();

    if (skip) {
        skip = false;
        return;
    }

    if (edgeTimeStamp[2]-edgeTimeStamp[1] < 200) {
        // Last edge was too short.
        // Skip this edge, and the next too.
        skip = true;
        return;
    }

    arg->pulse = edgeTimeStamp[1] - edgeTimeStamp[0];
    edgeTimeStamp[0] = edgeTimeStamp[1];
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

    while ((i >= 0) && (timings[i] >= pulse_length*0.6) && (timings[i] <= pulse_length*1.4)) {
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
   static int in_loop = 0;
//    unsigned int i;

//    cli();

    if (in_loop)
    {
        // avoid re-entrance
        return;
    }

    unsigned long pulse = this->store_.pulse;
    this->store_.pulse = 0;

    in_loop = 1;

    if (pulse > 0)
    {
        if (pulse < 200)
        {
            // noise
            idx = 0;
        }
        else
        {
            this->store_.timings_data[idx] = pulse;
            idx++;
            if (idx >= BUFFSIZE)
            {
                idx = 0;
            }


            if ((pulse > 15000) && (idx >= 32))
            {
                ESP_LOGD(TAG, "Pulse: %d idx: %d", pulse, idx);
    
                detect_pulse_length(this->store_.timings_data, idx);
                // decode_ook_ppm_nexus(this->store_.timings_data, idx);

                idx = 0;
            }
        }
    }

    in_loop = 0;

//    sei();
}

void EmptySensor::dump_config()
{
    LOG_SENSOR("", "Empty Sensor", this);
    LOG_PIN("  Pin: ", this->pin_);
}

} // namespace empty_sensor
} // namespace esphome