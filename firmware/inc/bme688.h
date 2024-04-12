#ifndef BME688_H
#define BME688_H

#include <stdint.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <hardware/spi.h>
#include <math.h> // for atan.


class BME688
{
public:
    enum mode: uint8_t
    {
        SLEEP = 0x00,
        FORCED = 0x01, // a one-shot mode that returns to sleep mode afterwards.
        PARALLEL = 0x02
    };

/**
 * \brief Constructor that will also initialize spi hardware and pins
 */
    BME688(spi_inst_t* spi_hw,
           uint8_t spi_tx_pin, uint8_t spi_rx_pin, uint8_t spi_sck_pin,
           uint8_t cs_pin, bool init_spi_hardware = true);
/**
 * \brief Convenience constructor that's more useful if the SPI peripheral
 *      and pins were already initialized.
 */
    BME688(spi_inst_t* spi_hw, uint8_t cs_pin);

    ~BME688();

    static void init_spi(spi_inst_t* spi);

    void write_to_reg(RegName reg, uint16_t value);

    void init_one_shot();

    void init_continuous_measurement();

    void get_temperature();
    void get_pressure();
    void get_humidity();
    void get_gas();

/**
 * \brief
 */
    void reset();

private:
    void spi_write8(uint16_t word);

    void mode_;  /// Sensor mode.

    uint8_t cs_pin_;
    spi_inst_t* spi_inst_;
};
#endif // BME688_H
