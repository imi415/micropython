#ifndef BOARD_H
#define BOARD_H

#include "py/mphal.h"

typedef void (*mcx_board_isr_t)(void *param);

void MCX_BoardEarlyInit(void);

/* UART */
void *MCX_BoardGetUARTInstance(uint8_t id);
int MCX_BoardConfigureUARTClock(uint8_t id);
int MCX_BoardConfigureUARTISR(uint8_t id, mcx_board_isr_t isr, void *param);

/* OSTimer */
#ifdef MICROPY_HW_HAS_OSTIMER
void *MCX_BoardGetOSTimerInstance(uint8_t id);
int MCX_BoardConfigureOSTimerClock(uint8_t id);
#endif

#endif