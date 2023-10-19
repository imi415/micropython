#ifndef DRV_UART_H
#define DRV_UART_H

#include <stdint.h>

typedef enum {
    DRV_Uart7DataBits,
    DRV_Uart8DataBits,
} drv_uart_data_bits_t;

typedef enum {
    DRV_Uart1StopBits,
    DRV_Uart2StopBits,
} drv_uart_stop_bits_t;

typedef enum {
    DRV_UartNoParity,
    DRV_UartEvenParity,
    DRV_UartOddParity,
} drv_uart_parity_t;

typedef struct drv_uart_type {
    uint8_t id;
    void *instance;
} drv_uart_t;

typedef struct drv_uart_config_type {
    uint32_t baud_rate;
    drv_uart_data_bits_t data_bits;
    drv_uart_stop_bits_t stop_bits;
    drv_uart_parity_t parity;
} drv_uart_config_t;

int drv_uart_init(drv_uart_t *uart, uint8_t id, drv_uart_config_t *cfg);
int drv_uart_write(drv_uart_t *uart, const uint8_t *data, int len);
int drv_uart_read(drv_uart_t *uart, uint8_t *data, int len);

#endif