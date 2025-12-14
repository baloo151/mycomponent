#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/sensor/sensor.h"
#include "bitbuffer.h"

#define BUFFSIZE 1024

namespace esphome {
namespace empty_sensor {

/// Store data in a class that doesn't use multiple-inheritance (vtables in flash)
class EmptySensorStore {
    public:
        void setup(InternalGPIOPin *pin) {
            pin->setup();
            this->pin_ = pin->to_isr();
            pin->attach_interrupt(&EmptySensorStore::gpio_intr, this, gpio::INTERRUPT_ANY_EDGE);
        }
        static void gpio_intr(EmptySensorStore *arg);

    protected:
        ISRInternalGPIOPin pin_;
        volatile unsigned long pulse = 0;
        unsigned int timings_data[BUFFSIZE];
        bitbuffer_t bits = {0};
};


class EmptySensor : public sensor::Sensor, public Component {
    public:
        void set_pin(InternalGPIOPin *pin) { pin_ = pin; }
        void setup() override{ this->store_.setup(this->pin_); }
        void loop() override;
        void dump_config() override;

    protected:
        EmptySensorStore store_;
        InternalGPIOPin *pin_;
};

} //namespace empty_sensor
} //namespace esphome