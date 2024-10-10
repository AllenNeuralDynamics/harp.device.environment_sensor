#include <pico/stdlib.h> // for uart printing
#include <cstring>
#include <stdio.h>
#include <config.h>
#include <pico/stdio_usb.h>
#include <stdio.h>
#include "Adafruit_BME680_rpi.h"


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


// Core0 main.
int main()
{

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

  while(true) {
    loop(bme688);
    gpio_put(LED_PIN, 1);
    gpio_put(HARP_CORE_LED_PIN, 0);
    sleep_ms(500);
    gpio_put(LED_PIN, 0);
    gpio_put(HARP_CORE_LED_PIN, 1);
    sleep_ms(500);
        
    }
}
