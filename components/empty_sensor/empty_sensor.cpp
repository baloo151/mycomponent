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

void decode_ook_ppm_nexus(EmptySensorStore *arg, unsigned int count)
{
   unsigned int pulse_l = arg->timings_data[count-2] * 0.8;
   unsigned int pulse_u = arg->timings_data[count-2] * 1.2;
   unsigned int zero_l = 1000 * 0.8;
   unsigned int zero_u = 1000 * 1.2;
   unsigned int one_l = 2000 * 0.8;
   unsigned int one_u = 2000 * 1.2;
   unsigned int reset = 5000;

   int state = OOK_EXPECT_PULSE;

   bitbuffer_clear(&arg->bits);

   unsigned int nbits = 0;

   for (unsigned int i = 0; i < count; i++)
   {
      switch (state)
      {
      case OOK_EXPECT_PULSE:
         if ((arg->timings_data[i] > pulse_l) && (arg->timings_data[i] < pulse_u))
         {
            state = OOK_EXPECT_GAP;
         }
         break;
      case OOK_EXPECT_GAP:
         if ((arg->timings_data[i] > zero_l) && (arg->timings_data[i] < zero_u))
         {
            bitbuffer_add_bit(&arg->bits, 0);
            nbits++;
         }
         else if ((arg->timings_data[i] > one_l) && (arg->timings_data[i] < one_u))
         {
            bitbuffer_add_bit(&arg->bits, 1);
            nbits++;
         }
         else if (arg->timings_data[i] < reset)
         {
            if (nbits >= 16)
            {
               bitbuffer_add_row(&arg->bits);
               nbits = 0;
            }
         }
         state = OOK_EXPECT_PULSE;
      }
   }
   int row = bitbuffer_find_repeated_row(&arg->bits, 3, 36);
   if (row < 0)
      return;

   if (arg->bits.bits_per_row[row] == 36)
   {
      uint8_t *b;

      b = arg->bits.bb[row];

      int id = b[0];
      int battery = b[1] & 0x80;
      int channel = ((b[1] & 0x30) >> 4) + 1;
      int temp_raw = (int16_t)((b[1] << 12) | (b[2] << 4)); // sign-extend
      float temp_c = (temp_raw >> 4) * 0.1f;
      int humidity = (((b[3] & 0x0F) << 4) | (b[4] >> 4));

      ESP_LOGD(TAG, "Nexus id: %d; battery_ok: %d; channel: %d; temp: %.1f C; humidity: %d%%",
               id, !!battery, channel, temp_c, humidity);
   }
}

void EmptySensor::loop()
{
    // this->publish_state(counter); // Publie la valeur du compteur sur le capteur

    static unsigned int idx = 0;
    static int in_loop = 0;

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
        this->store_.timings_data[idx] = pulse;
        idx++;
        if (idx >= BUFFSIZE)
        {
            idx = 0;
        }


        if ((pulse > 15000) && (idx >= 32))
        {
            decode_ook_ppm_nexus(&this->store_, idx);

            idx = 0;
        }
    }

    in_loop = 0;
}

void EmptySensor::dump_config()
{
    LOG_SENSOR("", "Empty Sensor", this);
    LOG_PIN("  Pin: ", this->pin_);
}

} // namespace empty_sensor
} // namespace esphome