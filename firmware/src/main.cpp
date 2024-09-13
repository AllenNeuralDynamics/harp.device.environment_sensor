#include <pico/stdlib.h> // for uart printing
#include <cstring>
#include <stdio.h>
// #include <harp_c_app.h>
// #include <harp_synchronizer.h>
// #include <core_registers.h>
// #include <reg_types.h>
// #include <bme68x.h>
#include "Adafruit_BME680_rpi.h"
#include <config.h>
// #include <hardware/spi.h>
#ifdef DEBUG
    // #include <cstdio> // for printf
    #include <stdio.h>
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

#ifndef DEBUG_SPI

Adafruit_BME680 bme688(spi0,BME688_PICO_PIN,BME688_POCI_PIN,BME688_SCK_PIN,BME688_CS_PIN,true);


#endif

//////// Harp stuff

// // Harp App Register Setup.
// const size_t reg_count = 3;

// // Define register contents.
// #pragma pack(push, 1)
// struct app_regs_t
// {
//     uint16_t pressure_kpa;  // app register 0
//     uint16_t temperature_c;         // app register 1
//     uint16_t humidity_units;         // app register 1
// } app_regs;
// #pragma pack(pop)

// // Define register "specs."
// RegSpecs app_reg_specs[reg_count]
// {
//     {(uint8_t*)&app_regs.pressure_kpa, sizeof(app_regs.pressure_kpa), U16},
//     {(uint8_t*)&app_regs.temperature_c, sizeof(app_regs.temperature_c), U16},
//     {(uint8_t*)&app_regs.humidity_units, sizeof(app_regs.humidity_units), U16}
// };

// // Define register read-and-write handler functions.
// RegFnPair reg_handler_fns[reg_count]
// {
//     {&HarpCore::read_reg_generic, &HarpCore::write_reg_generic},
//     {&HarpCore::read_reg_generic, &HarpCore::write_to_read_only_reg_error}
// };

// void app_reset()
// {
//     app_regs.pressure_kpa = 0;
//     app_regs.temperature_c = 0;
//     app_regs.humidity_units = 0;
// }

// void update_app_state()
// {
//     // update here!
//     // If app registers update their states outside the read/write handler
//     // functions, update them here.
//     // (Called inside run() function.)
// }

// // Create Harp App.
// HarpCApp& app = HarpCApp::init(who_am_i, hw_version_major, hw_version_minor,
//                                assembly_version,
//                                harp_version_major, harp_version_minor,
//                                fw_version_major, fw_version_minor,
//                                serial_number, "Env Sensor",
//                                &app_regs, app_reg_specs,
//                                reg_handler_fns, reg_count, update_app_state,
//                                app_reset);


/////////

// copied from https://github.com/adafruit/Adafruit_BME680/blob/master/examples/bme680test/bme680test.ino
void loop(Adafruit_BME680& bme) {
  if (! bme.performReading()) {
    printf("Failed to perform reading :(\r\n");
    return;
  }
  printf("Temperature = %f *C\r\n", bme.temperature);

  printf("Pressure = %f hPa\r\n", bme.pressure/100);

  printf("Humidity = %f %\r\n", bme.humidity);

  printf("Gas = %f KOhms\r\n", bme.gas_resistance/1000.0);

  printf("\n");
  sleep_ms(2000);
}


#ifdef DEBUG_SPI
///////////////////
// Copied from https://github.com/boschsensortec/BME68x_SensorAPI/blob/master/examples/forced_mode/forced_mode.c

#define SAMPLE_COUNT  UINT16_C(300)

void bme68x_check_rslt(const char api_name[], int8_t rslt)
{
    if (rslt != BME68X_OK) {
        printf("Bad result in %c\r\n",api_name);
    }
}


int8_t manual(MySPI *spidev) {

    // struct bme68x_dev bme = adafruit_bme.gas_sensor;
    struct bme68x_dev bme;
    int8_t rslt;
    struct bme68x_conf conf;
    struct bme68x_heatr_conf heatr_conf;
    struct bme68x_data data;
    uint32_t del_period;
    uint32_t time_ms = 0;
    uint8_t n_fields;
    uint16_t sample_count = 1;

    
    bme.chip_id = 0;
    bme.intf = BME68X_SPI_INTF;
    bme.intf_ptr = (void *)spidev;
    bme.read = &spi_read;
    bme.write = &spi_write;

    bme.amb_temp = 25; /* The ambient temperature in deg C is used for
                                defining the heater temperature */
    bme.delay_us = delay_usec;
    
    rslt = bme68x_init(&bme);
    bme68x_check_rslt("bme68x_init", rslt);

    /* Check if rslt == BME68X_OK, report or handle if otherwise */
    conf.filter = BME68X_FILTER_OFF;
    conf.odr = BME68X_ODR_NONE;
    conf.os_hum = BME68X_OS_16X;
    conf.os_pres = BME68X_OS_1X;
    conf.os_temp = BME68X_OS_2X;
    rslt = bme68x_set_conf(&conf, &bme);
    bme68x_check_rslt("bme68x_set_conf", rslt);

    /* Check if rslt == BME68X_OK, report or handle if otherwise */
    heatr_conf.enable = BME68X_ENABLE;
    heatr_conf.heatr_temp = 300;
    heatr_conf.heatr_dur = 100;
    rslt = bme68x_set_heatr_conf(BME68X_FORCED_MODE, &heatr_conf, &bme);
    bme68x_check_rslt("bme68x_set_heatr_conf", rslt);

    printf("Sample, TimeStamp(ms), Temperature(deg C), Pressure(Pa), Humidity(%%), Gas resistance(ohm), Status\n");

    while (sample_count <= SAMPLE_COUNT)
    {
        rslt = bme68x_set_op_mode(BME68X_FORCED_MODE, &bme);
        bme68x_check_rslt("bme68x_set_op_mode", rslt);

        /* Calculate delay period in microseconds */
        del_period = bme68x_get_meas_dur(BME68X_FORCED_MODE, &conf, &bme) + (heatr_conf.heatr_dur * 1000);
        bme.delay_us(del_period, bme.intf_ptr);

        // time_ms = coines_get_millis();
        time_ms = millis();

        /* Check if rslt == BME68X_OK, report or handle if otherwise */
        rslt = bme68x_get_data(BME68X_FORCED_MODE, &data, &n_fields, &bme);
        bme68x_check_rslt("bme68x_get_data", rslt);

        if (n_fields)
        {
#ifdef BME68X_USE_FPU
            printf("%u, %lu, %.2f, %.2f, %.2f, %.2f, 0x%x\n",
                   sample_count,
                   (long unsigned int)time_ms,
                   data.temperature,
                   data.pressure,
                   data.humidity,
                   data.gas_resistance,
                   data.status);
#else
            printf("%u, %lu, %d, %lu, %lu, %lu, 0x%x\n",
                   sample_count,
                   (long unsigned int)time_ms,
                   (data.temperature / 100),
                   (long unsigned int)data.pressure,
                   (long unsigned int)(data.humidity / 1000),
                   (long unsigned int)data.gas_resistance,
                   data.status);
#endif
            sample_count++;
        }
    }

    return rslt;
}

///////////////////////////
#endif

// Core0 main.
int main()
{
// Init Synchronizer.
    // HarpSynchronizer& sync = HarpSynchronizer::init(HARP_SYNC_UART_ID,
    //                                                 HARP_SYNC_RX_PIN);
    // // app.set_visual_indicators_fn(set_led_state);
    // app.set_synchronizer(&sync);
#ifdef DEBUG
    // stdio_uart_init_full(DEBUG_UART_ID, DEBUG_UART_BAUDRATE,
    //                      DEBUG_UART_TX_PIN, -1); // use TX only.
    stdio_init_all();
    printf("Hello, from an RP2040!\r\n");
#endif
    stdio_init_all();
    // Wait for serial port to open before continuing.
    while (!stdio_usb_connected());
    sleep_ms(1000);
    printf("Hello, from an RP2040!\r\n");
    set_led_state(true);
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_init(HARP_CORE_LED_PIN);
    gpio_set_dir(HARP_CORE_LED_PIN, GPIO_OUT);
    sleep_ms(5000);
    gpio_put(LED_PIN, 1);
    sleep_ms(2000);
    printf("Hello, from an RP2040!\r\n");

#ifdef DEBUG_SPI
    // MySPI spi(spi0,BME688_PICO_PIN,BME688_POCI_PIN,BME688_SCK_PIN,BME688_CS_PIN,true);
    MySPI* spi = new MySPI(spi0,BME688_PICO_PIN,BME688_POCI_PIN,BME688_SCK_PIN,BME688_CS_PIN,true);
    // MySPI *spidev = MySPI(spi0,BME688_PICO_PIN,BME688_POCI_PIN,BME688_SCK_PIN,BME688_CS_PIN,true);
    uint8_t addr = 0x50;
    uint8_t data;
    // addr = 0x00;
    manual(spi);
#else

    int8_t rslt = bme688.begin(0,true);

#endif

    while(true) {


#ifdef DEBUG_SPI

        spi_read(0x73,&data,1,(void *)spi);

        addr = 0x50;
        spi->read(addr,&data,1);
        printf("Chip ID (addr %x) is %x\r\n",addr, data);

        // check variant ID
        spi->read(0x70,&data,1);
        printf("Variant ID (addr 0x70) is %x\r\n",data);

        spi->read(0x73,&data,1);
        printf("Status Reg at %x is %x\r\n",0xf3, data);

        data = 0x10;
        spi->write(0x73,&data,1);

        spi->read(0x73,&data,1);
        printf("Status Reg at %x is %x\r\n",0xf3, data);

        data = 0x00;
        spi->write(0x73,&data,1);
#else
        loop(bme688);
        // app.run();
#endif
        
        // 
        gpio_put(LED_PIN, 1);
        gpio_put(HARP_CORE_LED_PIN, 0);
        // app.run();
        sleep_ms(500);
        gpio_put(LED_PIN, 0);
        gpio_put(HARP_CORE_LED_PIN, 1);
        sleep_ms(500);
        // printf("Hello, bme680.begin result is %i\r\n", rslt);
#ifdef DEBUG
    // stdio_uart_init_full(DEBUG_UART_ID, DEBUG_UART_BAUDRATE,
    //                      DEBUG_UART_TX_PIN, -1); // use TX only.
    // stdio_init_all();
    printf("Hello, from an RP2040!\r\n");
#endif
    }
}
