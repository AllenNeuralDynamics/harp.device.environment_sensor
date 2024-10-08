#include <pico/stdlib.h> // for uart printing
#include <cstring>
#include <stdio.h>
#include <config.h>
#ifdef DEBUG
    #include <pico/stdio_usb.h>
    #include <stdio.h>
    #include "Adafruit_BME680_rpi.h"
#else
    #include <pico/multicore.h>
    #include <harp_c_app.h>
    #include <harp_synchronizer.h>
    #include <core_registers.h>
    #include <reg_types.h>
    #include "core1_sensor.h"
#endif

// Create device name array.
const uint16_t who_am_i = 1845;
const uint8_t hw_version_major = 1;
const uint8_t hw_version_minor = 0;
const uint8_t assembly_version = 2;
const uint8_t harp_version_major = 2;
const uint8_t harp_version_minor = 0;
const uint8_t fw_version_major = 3;
const uint8_t fw_version_minor = 0;
const uint16_t serial_number = ENV_SENSOR_DEVICE_ID;

const uint16_t read_period_us = 5000;

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



#ifdef DEBUG

Adafruit_BME680 bme688(spi0,BME688_PICO_PIN,BME688_POCI_PIN,BME688_SCK_PIN,BME688_CS_PIN,true);

// copied from https://github.com/adafruit/Adafruit_BME680/blob/master/examples/bme680test/bme680test.ino
void loop(Adafruit_BME680& bme) {
  uint32_t ping = time_us_32();
  if (! bme.performReading()) {
    printf("Failed to perform reading :(\r\n");
    return;
  }
  uint32_t pong = time_us_32();
  uint32_t time = pong - ping;

  printf("Reading time = %lu us\r\n", time);

  printf("Temperature = %f *C\r\n", bme.temperature);

  printf("Pressure = %i Pa\r\n", bme.pressure);

  printf("Humidity = %f %\r\n", bme.humidity);

  printf("Gas = %f KOhms\r\n", bme.gas_resistance/1000.0);

  printf("\n");
  sleep_ms(2000);
}

#else

queue_t sensor_queue;

//////// Harp stuff

// Harp App Register Setup.
const size_t reg_count = 3;

// Define register contents.
#pragma pack(push, 1)
struct app_regs_t
{
    uint32_t pressure_pa;  // app register 0
    float temperature_c;         // app register 1
    float humidity_units;         // app register 2
} app_regs;
#pragma pack(pop)

// Define register "specs."
RegSpecs app_reg_specs[reg_count]
{
    {(uint8_t*)&app_regs.pressure_pa, sizeof(app_regs.pressure_pa), U32},
    {(uint8_t*)&app_regs.temperature_c, sizeof(app_regs.temperature_c), Float},
    {(uint8_t*)&app_regs.humidity_units, sizeof(app_regs.humidity_units), Float}
};

// Define register read-and-write handler functions.
RegFnPair reg_handler_fns[reg_count]
{
    {&HarpCore::read_reg_generic, &HarpCore::write_to_read_only_reg_error},
    {&HarpCore::read_reg_generic, &HarpCore::write_to_read_only_reg_error},
    {&HarpCore::read_reg_generic, &HarpCore::write_to_read_only_reg_error}
};

void app_reset()
{
    app_regs.pressure_pa = 0;
    app_regs.temperature_c = 0;
    app_regs.humidity_units = 0;
}

void update_app_state()
{
    // Get data from sensor-reading loop via queue and set registers
    if (!queue_is_empty(&sensor_queue))
        queue_remove_blocking(&sensor_queue, &app_regs);

        // // Send an event for each register
        // for (int i=0; i < reg_count; i++) {
        //     const RegSpecs& reg_specs = app_reg_specs[i];
        //     if (!HarpCore::is_muted())
        //         HarpCApp::send_harp_reply(EVENT, APP_REG_START_ADDRESS, reg_specs.base_ptr,
        //                                 reg_specs.num_bytes, reg_specs.payload_type);
        // }



}

// Create Harp App.
HarpCApp& app = HarpCApp::init(who_am_i, hw_version_major, hw_version_minor,
                               assembly_version,
                               harp_version_major, harp_version_minor,
                               fw_version_major, fw_version_minor,
                               serial_number, "Env Sensor",
                               &app_regs, app_reg_specs,
                               reg_handler_fns, reg_count, update_app_state,
                               app_reset);


/////////
#endif

// Core0 main.
int main()
{

#ifdef DEBUG

    stdio_init_all();
    // Wait for serial port to open before continuing.
    // while (!stdio_usb_connected());
    sleep_ms(1000);
    printf("Hello, from an RP2040!\r\n");
    set_led_state(true);
    gpio_init(LED_PIN);
    gpio_init(HARP_CORE_LED_PIN);
    gpio_set_dir(HARP_CORE_LED_PIN, GPIO_OUT);
    sleep_ms(5000);
    gpio_put(LED_PIN, 1);
    sleep_ms(2000);
    printf("Hello, from an RP2040!\r\n");

    int8_t rslt = bme688.begin(0,true);
#else
    

// Init Synchronizer.
    HarpSynchronizer& sync = HarpSynchronizer::init(HARP_SYNC_UART_ID,
                                                    HARP_SYNC_RX_PIN);
    // app.set_visual_indicators_fn(set_led_state);
    app.set_synchronizer(&sync);

    // launch core1 to read from the environment sensor
    bool core1_rslt = core1_setup();
    multicore_launch_core1(core1_main);

#endif


    while(true) {

#ifdef DEBUG
        loop(bme688);
        gpio_put(LED_PIN, 1);
        gpio_put(HARP_CORE_LED_PIN, 0);
        sleep_ms(500);
        gpio_put(LED_PIN, 0);
        gpio_put(HARP_CORE_LED_PIN, 1);
        sleep_ms(500);
#else
        app.run();
#endif
        
    }
}
