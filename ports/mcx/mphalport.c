#include "py/runtime.h"
#include "py/stream.h"
#include "py/mphal.h"
#include "extmod/misc.h"

MP_WEAK void mp_hal_stdout_tx_strn(const char *str, size_t len) {

}

MP_WEAK int mp_hal_stdin_rx_chr(void) {
    for(;;) {
        // TODO: Here.
    }
}