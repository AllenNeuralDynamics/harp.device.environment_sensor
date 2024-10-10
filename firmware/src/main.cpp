#include <pico/stdlib.h> // for uart printing
#include <cstring>
#include <stdio.h>
#include <config.h>
#include <pico/multicore.h>
#include <harp_c_app.h>
#include <harp_synchronizer.h>
#include <core_registers.h>
#include <reg_types.h>
#include "core1_sensor.h"

// Create device name array.
const uint16_t who_am_i = 1845;
const uint8_t hw_version_major = 1;
const uint8_t hw_version_minor = 0;
const uint8_t assembly_version = 2;
const uint8_t harp_version_major = 0;
const uint8_t harp_version_minor = 0;
const uint8_t fw_version_major = 0;
const uint8_t fw_version_minor = 1;
const uint16_t serial_number = ENV_SENSOR_DEVICE_ID;


void set_led_state(bool enabled)
{
    if (enabled)
    {
        gpio_init(LED_PIN);
        gpio_set_dir(LED_PIN, true); // true for output
        gpio_put(LED_PIN, 0);
    }
    else
        gpio_deinit(LED_PIN);
}


queue_t sensor_queue;
queue_t cmd_queue;

//////// Harp stuff

// Harp App Register Setup.
const size_t reg_count = 5;

// Define register contents.
#pragma pack(push, 1)
struct app_regs_t
{
    uint32_t pressure_pa;  // app register 0
    float temperature_c;         // app register 1
    float humidity_units;         // app register 2
    float pressure_temp_humidity[3];
    uint8_t enable_sensor_dispatch_events;
    // bool mute_harp_events; // harp.ismuted()?
} app_regs;
#pragma pack(pop)

// Define register "specs."
RegSpecs app_reg_specs[reg_count]
{
    {(uint8_t*)&app_regs.pressure_pa, sizeof(app_regs.pressure_pa), U32},
    {(uint8_t*)&app_regs.temperature_c, sizeof(app_regs.temperature_c), Float},
    {(uint8_t*)&app_regs.humidity_units, sizeof(app_regs.humidity_units), Float},
    {(uint8_t*)&app_regs.pressure_temp_humidity, sizeof(app_regs.pressure_temp_humidity), Float},
    {(uint8_t*)&app_regs.enable_sensor_dispatch_events, sizeof(app_regs.enable_sensor_dispatch_events), U8}
};

// Define register read-and-write handler functions.
RegFnPair reg_handler_fns[reg_count]
{
    {&HarpCore::read_reg_generic, &HarpCore::write_to_read_only_reg_error},
    {&HarpCore::read_reg_generic, &HarpCore::write_to_read_only_reg_error},
    {&HarpCore::read_reg_generic, &HarpCore::write_to_read_only_reg_error},
    {&HarpCore::read_reg_generic, &HarpCore::write_to_read_only_reg_error},
    {&HarpCore::read_reg_generic, &HarpCore::write_reg_generic},
};

void app_reset()
{
    app_regs.pressure_pa = 0;
    app_regs.temperature_c = 0;
    app_regs.humidity_units = 0;
    app_regs.pressure_temp_humidity[0] = 0;
    app_regs.pressure_temp_humidity[1] = 0;
    app_regs.pressure_temp_humidity[2] = 0;

    app_regs.enable_sensor_dispatch_events = 1;

    uint8_t reset = 0xff;
    queue_add_blocking(&cmd_queue,&reset);
}

void update_app_state()
{
    // Get data from sensor-reading loop via queue and set registers
    if (!queue_is_empty(&sensor_queue)){
        sensor_data_t data;
        queue_remove_blocking(&sensor_queue, &data);

        app_regs.pressure_pa = data.pressure_pa;
        app_regs.temperature_c = data.temperature_c;
        app_regs.humidity_units = data.humidity_units;
        app_regs.pressure_temp_humidity[0] = float(data.pressure_pa);
        app_regs.pressure_temp_humidity[1] = data.temperature_c;
        app_regs.pressure_temp_humidity[2] = data.humidity_units;
        
        // Send an event for the aggregate register
        if (!HarpCore::is_muted() && bool(app_regs.enable_sensor_dispatch_events))
            HarpCApp::send_harp_reply(EVENT, APP_REG_START_ADDRESS + 3);


    }

}

// Create Harp App.
HarpCApp& app = HarpCApp::init(who_am_i, hw_version_major, hw_version_minor,
                               assembly_version,
                               harp_version_major, harp_version_minor,
                               fw_version_major, fw_version_minor,
                               serial_number, "Env Sensor",
                               (const uint8_t*)GIT_HASH, // in CMakeLists.txt.
                               &app_regs, app_reg_specs,
                               reg_handler_fns, reg_count, update_app_state,
                               app_reset);



// Core0 main.
int main()
{
 

// Init Synchronizer.
    HarpSynchronizer& sync = HarpSynchronizer::init(HARP_SYNC_UART_ID,
                                                    HARP_SYNC_RX_PIN);
    // app.set_visual_indicators_fn(set_led_state);
    app.set_synchronizer(&sync);

    // launch core1 to read from the environment sensor
    bool core1_rslt = core1_setup();
    multicore_launch_core1(core1_main);

    // enable events by default
    app_regs.enable_sensor_dispatch_events = 1;

    while(true) {

        app.run();
        
    }
}
