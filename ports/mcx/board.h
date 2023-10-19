#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

void MCX_BoardEarlyInit(void);
void *MCX_BoardGetUARTInstanceById(uint8_t id);
int MCX_BoardConfigureUARTClock(void *instance);

#endif