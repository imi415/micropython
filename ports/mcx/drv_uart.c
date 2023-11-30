#include "board.h"

/* SDK drivers */
#include "fsl_lpuart.h"

#include "drv_uart.h"

int drv_uart_init(drv_uart_t *uart, uint8_t id, drv_uart_config_t *cfg) {
    uart->instance = MCX_BoardGetUARTInstance(id);
    if(uart->instance == NULL) {
        return -1;
    }

    int uart_inputfreq = MCX_BoardConfigureUARTClock(id);
    if(uart_inputfreq <= 0) {
        return -2;
    }

    lpuart_config_t uart_cfg;
    LPUART_GetDefaultConfig(&uart_cfg);

    uart_cfg.baudRate_Bps = cfg->baud_rate;
    uart_cfg.enableRx = true;
    uart_cfg.enableTx = true;

#if defined(FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT) && FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT
    if(cfg->data_bits == DRV_Uart7DataBits) {
        uart_cfg.dataBitsCount = kLPUART_SevenDataBits;
    }
#endif
    if(cfg->data_bits == DRV_Uart8DataBits) {
        uart_cfg.dataBitsCount = kLPUART_EightDataBits;
    }

    switch(cfg->parity) {
        case DRV_UartNoParity:
            uart_cfg.parityMode = kLPUART_ParityDisabled;
            break;
        case DRV_UartEvenParity:
            uart_cfg.parityMode = kLPUART_ParityEven;
            break;
        case DRV_UartOddParity:
            uart_cfg.parityMode = kLPUART_ParityOdd;
            break;
        default:
            break;
    }

    if(LPUART_Init(uart->instance, &uart_cfg, uart_inputfreq) != kStatus_Success) {
        return -3;
    }

    return 0;
}

int drv_uart_write(drv_uart_t *uart, const uint8_t *data, int len) {
    if(LPUART_WriteBlocking(uart->instance, data, len) != kStatus_Success) {
        return -1;
    }

    return len;
}

int drv_uart_read(drv_uart_t *uart, uint8_t *data, int len) {
    if(LPUART_ReadBlocking(uart->instance, data, len) != kStatus_Success) {
        return -1;
    }

    return len;
}