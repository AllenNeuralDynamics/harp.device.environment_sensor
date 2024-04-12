#include <bme688.h>

BME688::BME688(spi_inst_t* spi_hw,
               uint8_t spi_tx_pin, uint8_t spi_rx_pin, uint8_t spi_sck_pin,
               uint8_t cs_pin, bool init_spi_hardware)
:BME688(spi_hw, cs_pin)
{
    // init spi if specified to do so.
    if (init_spi_hardware)
        init_spi(spi_inst_);

    gpio_init(spi_sck_pin);
    gpio_set_dir(spi_sck_pin, GPIO_OUT);
    // Set chip select HIGH at the beginning.
    gpio_put(spi_sck_pin, 1);

    // Delegate cs_pin setup to another constructor.
    // Setup pin modes for SPI.
    gpio_set_function(spi_tx_pin, GPIO_FUNC_SPI);
    gpio_set_function(spi_rx_pin, GPIO_FUNC_SPI);
    gpio_set_function(spi_sck_pin, GPIO_FUNC_SPI);
}

BME688::BME688(spi_inst_t* spi_hw, uint8_t cs_pin)
:spi_inst_{spi_hw}, cs_pin_{cs_pin}
{
    // Setup chip select.
    gpio_init(cs_pin_);
    gpio_set_dir(cs_pin_, GPIO_OUT);
    // Set chip select HIGH at the beginning.
    gpio_put(cs_pin_, 1);
}

void BME688::init_spi(spi_inst_t* spi_hw)
{
    // Setup SPI mode 0 (CPOL=0, CPHA=0), SPI clock speed to <=10[MHz].
    spi_init(spi_hw, 1000000U); // defaults to controller mode.
    spi_set_format(spi_hw,
                   8, // data bits
                   SPI_CPOL_0,
                   SPI_CPHA_0,
                   SPI_MSB_FIRST);
}

void BME688::init_continuous_measurement()
{
    // Set oversampling values for temperature, pressure, humidity.
    // Set IIR filter for temperature.
    // Enable gas conversion.
    // Select number of steps in heater profile
    // Set heater on-timer per step.
    // Set heater temperature of each step.
    // Set mode to parallel.
}

// Bit D15 and D14 give the address of the register.
void BME688::spi_write8(uint8_t word)
{
    // CS LOW
    asm volatile("nop");
    gpio_put(cs_pin_, 0);
    asm volatile("nop");
    // Send the data MSB first. (Note: RP2040 is little-endian.)
    spi_write8_blocking(spi_inst_, &word, 1);
    // CS HIGH
    asm volatile("nop");
    gpio_put(cs_pin_, 1);
    asm volatile("nop");
    //printf("Writing 0x%04x\n", word);
}

void BME688::write_to_reg(RegName reg, uint16_t value)
{
    uint16_t word = (uint16_t(reg) << 13) + value;
    if (device_is_reset_ && reg == CONTROL)
        word |= (1 << 8);
    spi_write8(word);
}

void BME688::reset()
{
    //write_to_reg(CONTROL, 0x0100); // Write 1 to D8.
}
