import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
)

empty_sensor_ns = cg.esphome_ns.namespace("empty_sensor")
EmptySensor = empty_sensor_ns.class_(
    "EmptySensor",
    cg.Component,
    sensor.Sensor,
)

CONFIG_SCHEMA = sensor.sensor_schema(
    EmptySensor,
    unit_of_measurement=UNIT_CELSIUS,
    accuracy_decimals=1,
    device_class=DEVICE_CLASS_TEMPERATURE,
    state_class=STATE_CLASS_MEASUREMENT,
)


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
