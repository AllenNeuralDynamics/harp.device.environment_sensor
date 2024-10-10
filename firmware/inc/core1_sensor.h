#ifndef CORE1_SENSOR_H
#define CORE1_SENSOR_H

#include <config.h>
#include <pico/util/queue.h>
// #include <hardware/spi.h>
#include "Adafruit_BME680_rpi.h"
// #include <Adafruit_BME680.h>

#pragma pack(push, 1)
struct sensor_data_t
{
    uint32_t pressure_pa;  
    float temperature_c;   
    float humidity_units;  
};
#pragma pack(pop)

extern queue_t sensor_queue;
extern queue_t cmd_queue;

#define CMD_MASK_RESET         UINT32_C(1);

bool core1_setup();

void core1_main();

#endif