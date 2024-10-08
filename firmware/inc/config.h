#ifndef CONFIG_H
#define CONFIG_H

#define TTL_PIN (23)
#define LED_PIN (24)
#define HARP_CORE_LED_PIN (25)

// #define DEBUG /////////

#define BME688_CS_PIN (20)
#define BME688_POCI_PIN (16)
#define BME688_PICO_PIN (19)
#define BME688_SCK_PIN (18)

// this board doesn't have a debug uart
// #define DEBUG_UART_ID (uart0)
// #define DEBUG_UART_TX_PIN (0)
// #define DEBUG_UART_BAUDRATE (921600)

#define HARP_SYNC_UART_ID (uart1)
#define HARP_SYNC_RX_PIN (5)

#define ENV_SENSOR_DEVICE_ID (0x057C) // 1404

// Doesnt work yet:
//#define USBD_MANUFACTURER "The Allen Institute for Neural Dynamics"
//#define USBD_PRODUCT "Harp.Device.Environment-Sensor"

#endif // CONFIG_H
