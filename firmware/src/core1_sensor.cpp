#include "core1_sensor.h"

Adafruit_BME680 bme688(spi0,BME688_PICO_PIN,BME688_POCI_PIN,BME688_SCK_PIN,BME688_CS_PIN,true);
// Adafruit_BME680 bme688(BME688_CS_PIN,BME688_PICO_PIN,BME688_POCI_PIN,BME688_SCK_PIN);

uint32_t cmd_word;

bool core1_setup() {
  bool rslt = bme688.begin(0,true);

  queue_init(&sensor_queue,sizeof(sensor_data_t),32);
  // queue_init(&cmd_queue,sizeof(cmd_word),8);

  return rslt;
}

void core1_main()
{
  while(true) {
    // Receive command
    if (queue_try_remove(&cmd_queue,&cmd_word)) {
      if (cmd_word) {
        bme68x_soft_reset(&bme688.gas_sensor);
      }
    }

    bme688.performReading(); // reading takes ~ 400 ms
    
    sensor_data_t data;
    data.pressure_pa = bme688.pressure;
    data.temperature_c = bme688.temperature;
    data.humidity_units = bme688.humidity;
    queue_add_blocking(&sensor_queue,&data);
  }
}