#include "board.h"

void MCX_BoardGenericUARTISR(uint8_t id);

void LP_FLEXCOMM0_IRQHandler(void) {
    MCX_BoardGenericUARTISR(0);
}

void LP_FLEXCOMM1_IRQHandler(void) {
    MCX_BoardGenericUARTISR(1);
}

void LP_FLEXCOMM2_IRQHandler(void) {
    MCX_BoardGenericUARTISR(2);
}

void LP_FLEXCOMM3_IRQHandler(void) {
    MCX_BoardGenericUARTISR(3);
}

void LP_FLEXCOMM4_IRQHandler(void) {
    MCX_BoardGenericUARTISR(4);
}

void LP_FLEXCOMM5_IRQHandler(void) {
    MCX_BoardGenericUARTISR(5);
}

void LP_FLEXCOMM6_IRQHandler(void) {
    MCX_BoardGenericUARTISR(6);
}

void LP_FLEXCOMM7_IRQHandler(void) {
    MCX_BoardGenericUARTISR(7);
}

void LP_FLEXCOMM8_IRQHandler(void) {
    MCX_BoardGenericUARTISR(8);
}

void LP_FLEXCOMM9_IRQHandler(void) {
    MCX_BoardGenericUARTISR(9);
}
