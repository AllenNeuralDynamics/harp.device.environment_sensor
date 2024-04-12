#include <cstring>
#include <harp_c_app.h>
#include <harp_synchronizer.h>
#include <core_registers.h>
#include <reg_types.h>
#include <bme688.h>
#include <config.h>
#ifdef DEBUG
    #include <pico/stdlib.h> // for uart printing
    #include <cstdio> // for printf
#endif

// Create device name array.
const uint16_t who_am_i = 1234;
const uint8_t hw_version_major = 1;
const uint8_t hw_version_minor = 0;
const uint8_t assembly_version = 2;
const uint8_t harp_version_major = 2;
const uint8_t harp_version_minor = 0;
const uint8_t fw_version_major = 3;
const uint8_t fw_version_minor = 0;
const uint16_t serial_number = ENV_SENSOR_DEVICE_ID;

// Harp App Register Setup.
const size_t reg_count = 3;

// Define register contents.
#pragma pack(push, 1)
struct app_regs_t
{
    uint16_t pressure_kpa;  // app register 0
    uint16_t temperature_c;         // app register 1
    uint16_t humidity_units;         // app register 1
} app_regs;
#pragma pack(pop)

// Define register "specs."
RegSpecs app_reg_specs[reg_count]
{
    {(uint8_t*)&app_regs.pressure_kpa, sizeof(app_regs.pressure_kpa), U16},
    {(uint8_t*)&app_regs.temperature_c, sizeof(app_regs.temperature_c), U16},
    {(uint8_t*)&app_regs.humidity_units, sizeof(app_regs.humidity_units), U16}
};

// Define register read-and-write handler functions.
RegFnPair reg_handler_fns[reg_count]
{
    {&HarpCore::read_reg_generic, &HarpCore::write_reg_generic},
    {&HarpCore::read_reg_generic, &HarpCore::write_to_read_only_reg_error}
};

void app_reset()
{
    app_regs.presure_kpa = 0;
    app_regs.temperature_c = 0;
    app_regs.humidity_units = 0;
}

void update_app_state()
{
    // update here!
    // If app registers update their states outside the read/write handler
    // functions, update them here.
    // (Called inside run() function.)
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

// Core0 main.
int main()
{
// Init Synchronizer.
    HarpSynchronizer& sync = HarpSynchronizer::init(HARP_SYNC_UART_ID,
                                                    HARP_SYNC_RX_PIN);
    app.set_synchronizer(&sync);
#ifdef DEBUG
    stdio_uart_init_full(DEBUG_UART_ID, DEBUG_UART_BAUDRATE,
                         DEBUG_UART_TX_PIN, -1); // use TX only.
    printf("Hello, from an RP2040!\r\n");
#endif
    while(true)
        app.run();
}
