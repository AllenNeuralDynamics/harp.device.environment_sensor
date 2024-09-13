#ifndef MYSPI_H
#define MYSPI_H

#include <hardware/spi.h>
#include <pico/stdlib.h>
#include <stdio.h>

class MySPI {
 public:
  MySPI(spi_inst_t* spi_hw, uint8_t spi_pico_pin, uint8_t spi_poci_pin,
      uint8_t spi_sck_pin, uint8_t cs_pin, bool init_spi_hardware = true);

  bool read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len);
  bool write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len);

 private:
  spi_inst_t* spi_inst_;
  uint8_t cs_pin_;
};

#endif  // MYSPI_H