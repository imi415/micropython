#include "py/runtime.h"
#include "py/mphal.h"

#include "fsl_common.h"
#include "fsl_common_arm.h"

void mp_hal_delay_ms(mp_uint_t ms) {

}

void mp_hal_delay_us(mp_uint_t us) {

}

mp_uint_t mp_hal_ticks_us(void) {
    return 0;
}

mp_uint_t mp_hal_ticks_cpu(void) {
    return 0;
}

void SysTick_Handler(void) {

}