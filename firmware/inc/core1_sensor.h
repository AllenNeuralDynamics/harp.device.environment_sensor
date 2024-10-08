#ifndef CORE1_SENSOR_H
#define CORE1_SENSOR_H

#include <config.h>
#include <pico/util/queue.h>
#include "Adafruit_BME680_rpi.h"

struct sensor_data_t
{
    uint32_t pressure_pa;  
    float temperature_c;   
    float humidity_units;  
};

extern queue_t sensor_queue;

bool core1_setup();

void core1_main();

#endif