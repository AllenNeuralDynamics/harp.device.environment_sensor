#include "myspi.h"

MySPI::MySPI(spi_inst_t* spi_hw, uint8_t spi_pico_pin, uint8_t spi_poci_pin,
         uint8_t spi_sck_pin, uint8_t cs_pin, bool init_spi_hardware)
         :spi_inst_{spi_hw}, cs_pin_{cs_pin} {

  // Setup chip select.
  gpio_init(cs_pin_);
  gpio_set_dir(cs_pin_, GPIO_OUT);
  // Set chip select HIGH at the beginning.
  gpio_put(cs_pin_, 1);

  if (init_spi_hardware) {
    // spi_init(spi_inst_, 1000000U);
    spi_init(spi_inst_, 500 * 1000);
    // spi_set_format(spi_inst_,
    //                8,          // data bits
    //                SPI_CPOL_1,  // CPOL
    //                SPI_CPHA_0,  // CPHA
    //                SPI_MSB_FIRST);
  }

  // Delegate cs_pin setup to another constructor.
  // Setup pin modes for SPI.
  gpio_set_function(spi_pico_pin, GPIO_FUNC_SPI);
  gpio_set_function(spi_poci_pin, GPIO_FUNC_SPI);
  gpio_set_function(spi_sck_pin, GPIO_FUNC_SPI);

};

bool MySPI::read(uint8_t reg_addr, uint8_t* reg_data, uint32_t len) {
  reg_addr |= 0x80;

  // CS LOW
  asm volatile("nop");
  gpio_put(cs_pin_, 0);
  asm volatile("nop");

  // printf("reading addr %x into %x\r\n",reg_addr,reg_data);

  spi_write_blocking(spi_inst_, &reg_addr,1);
  // sleep_ms(10);
  spi_read_blocking(spi_inst_,0,reg_data,len);


  asm volatile("nop");
  gpio_put(cs_pin_, 1);
  asm volatile("nop");
  printf("read %d bytes %x starting at addr %x\r\n", len, *reg_data, reg_addr);
  return true;
};

bool MySPI::write(uint8_t reg_addr, const uint8_t* reg_data, uint32_t len) {
  uint8_t buf[1];
  buf[0] = reg_addr & 0x7f;  // remove read bit as this is a write
  //buf[1] = *reg_data;

  // CS LOW
  asm volatile("nop");
  gpio_put(cs_pin_, 0);
  asm volatile("nop");

  // printf("about to write\r\n");
  spi_write_blocking(spi_inst_,buf,1);
  spi_write_blocking(spi_inst_, reg_data, len);


  asm volatile("nop");
  gpio_put(cs_pin_, 1);
  asm volatile("nop");
  printf("wrote %d bytes %x starting at addr %x\r\n", len, *reg_data, reg_addr);
  return true;
};
