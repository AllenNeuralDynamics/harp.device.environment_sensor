/*!
 * @file Adafruit_BME680.h
 *
 * Adafruit BME680 temperature, humidity, barometric pressure and gas sensor
 * driver
 *
 * This is the documentation for Adafruit's BME680 driver for the
 * Arduino platform.  It is designed specifically to work with the
 * Adafruit BME680 breakout: https://www.adafruit.com/products/3660
 *
 * These sensors use I2C to communicate, 2 pins (SCL+SDA) are required
 * to interface with the breakout.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * Written by Ladyada for Adafruit Industries.
 *
 * BSD license, all text here must be included in any redistribution.
 *
 */

#ifndef __BME680_H__
#define __BME680_H__

// #include "Arduino.h"
#include <hardware/spi.h>
#include <math.h>
#include <pico/time.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include "myspi.h"

#include "bme68x.h"
// #include <Adafruit_I2CDevice.h>
// #include <Adafruit_SPIDevice.h>
// #include <Adafruit_Sensor.h>
// #include <Wire.h>

#define BME68X_DEFAULT_ADDRESS (0x77)    ///< The default I2C address
#define BME68X_DEFAULT_SPIFREQ (1000000) ///< The default SPI Clock speed

#define BME680_OS_16X BME68X_OS_16X   ///< Alias for BME680 existing examples
#define BME680_OS_8X BME68X_OS_8X     ///< Alias for BME680 existing examples
#define BME680_OS_4X BME68X_OS_4X     ///< Alias for BME680 existing examples
#define BME680_OS_2X BME68X_OS_2X     ///< Alias for BME680 existing examples
#define BME680_OS_1X BME68X_OS_1X     ///< Alias for BME680 existing examples
#define BME680_OS_NONE BME68X_OS_NONE ///< Alias for BME680 existing examples

#define BME680_FILTER_SIZE_127                                                 \
  BME68X_FILTER_SIZE_127 ///< Alias for BME680 existing examples
#define BME680_FILTER_SIZE_63                                                  \
  BME68X_FILTER_SIZE_63 ///< Alias for BME680 existing examples
#define BME680_FILTER_SIZE_31                                                  \
  BME68X_FILTER_SIZE_31 ///< Alias for BME680 existing examples
#define BME680_FILTER_SIZE_15                                                  \
  BME68X_FILTER_SIZE_15 ///< Alias for BME680 existing examples
#define BME680_FILTER_SIZE_7                                                   \
  BME68X_FILTER_SIZE_7 ///< Alias for BME680 existing examples
#define BME680_FILTER_SIZE_3                                                   \
  BME68X_FILTER_SIZE_3 ///< Alias for BME680 existing examples
#define BME680_FILTER_SIZE_1                                                   \
  BME68X_FILTER_SIZE_1 ///< Alias for BME680 existing examples
#define BME680_FILTER_SIZE_0                                                   \
  BME68X_FILTER_OFF ///< Alias for BME680 existing examples

// #define BME680_DEBUG2

// class SerialClass { 
//   public:
//     static void print(const char *arg);
//     static void print(char arg);
//     static void println(char arg);
// };

// char F(const char *arg);

/*! Adafruit_BME680 Class for both I2C and SPI usage.
 *  Wraps the Bosch library for Arduino usage
 */
class Adafruit_BME680 {
public:
  /** Value returned by remainingReadingMillis indicating no asynchronous
   * reading has been initiated by beginReading. **/
  static constexpr int reading_not_started = -1;
  /** Value returned by remainingReadingMillis indicating asynchronous reading
   * is complete and calling endReading will not block. **/
  static constexpr int reading_complete = 0;

  Adafruit_BME680(spi_inst_t* spi_hw,
           uint8_t spi_pico_pin, uint8_t spi_poci_pin, uint8_t spi_sck_pin,
           uint8_t cs_pin, bool init_spi_hardware = true);

  // Adafruit_BME680(TwoWire *theWire = &Wire);
  // Adafruit_BME680(int8_t cspin, SPIClass *theSPI = &SPI);
  // Adafruit_BME680(int8_t cspin, int8_t mosipin, int8_t misopin, int8_t sckpin);

  bool begin(uint8_t addr = BME68X_DEFAULT_ADDRESS, bool initSettings = true);
  float readTemperature();
  float readPressure();
  float readHumidity();
  uint32_t readGas();
  float readAltitude(float seaLevel);

  bool setTemperatureOversampling(uint8_t os);
  bool setPressureOversampling(uint8_t os);
  bool setHumidityOversampling(uint8_t os);
  bool setIIRFilterSize(uint8_t fs);
  bool setGasHeater(uint16_t heaterTemp, uint16_t heaterTime);
  bool setODR(uint8_t odr);

  // Perform a reading in blocking mode.
  bool performReading();

  uint32_t beginReading();

  bool endReading();

  int remainingReadingMillis();

  /** Temperature (Celsius) assigned after calling performReading() or
   * endReading() **/
  float temperature;
  /** Pressure (Pascals) assigned after calling performReading() or endReading()
   * **/
  uint32_t pressure;
  /** Humidity (RH %) assigned after calling performReading() or endReading()
   * **/
  float humidity;
  /** Gas resistor (ohms) assigned after calling performReading() or
   * endReading() **/
  uint32_t gas_resistance;

  // static int8_t spi_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);
  // static int8_t spi_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);

// private:

  MySPI *_spidev = NULL;

  int32_t _sensorID;
  uint32_t _meas_start = 0;
  uint16_t _meas_period = 0;

  struct bme68x_dev gas_sensor;
  struct bme68x_conf gas_conf;
  struct bme68x_heatr_conf gas_heatr_conf;
};

#endif


// unsigned long millis()
// // {
// //   return to_ms_since_boot(get_absolute_time());
// // }

// void delay(uint32_t ms)
// // {
// //   sleep_ms(ms);
// // }

// static int8_t spi_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len,
//                        void *interface);
// static int8_t spi_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len,
//                         void *interface);
// static void delay_usec(uint32_t us, void *intf_ptr);


// unsigned long millis();

// void delay(uint32_t ms);