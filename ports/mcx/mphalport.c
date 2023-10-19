#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "extmod/misc.h"

#include "drv_uart.h"

MP_WEAK uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    return mp_os_dupterm_poll(poll_flags);
}

MP_WEAK void mp_hal_stdout_tx_strn(const char *str, size_t len) {
    drv_uart_t *uart = MP_STATE_PORT(stdio_uart);
    drv_uart_write(uart, (const uint8_t *)str, len);
}

MP_WEAK int mp_hal_stdin_rx_chr(void) {
    drv_uart_t *uart = MP_STATE_PORT(stdio_uart);

    char ch = 0x00;
    drv_uart_read(uart, (uint8_t *)&ch, 1);

    return ch;
}

MP_REGISTER_ROOT_POINTER(struct drv_uart_type *stdio_uart);