#include "board.h"

#include "clock_config.h"
#include "pin_mux.h"

static LPUART_Type * const s_mcxn947_uart_inst_map[] = {
    LPUART0,
    LPUART1,
    LPUART2,
    LPUART3,
    LPUART4,
    LPUART5,
    LPUART6,
    LPUART7,
    LPUART8,
    LPUART9,
};

static clock_attach_id_t const s_mcxn947_flexcomm_clk_att_map[] = {
    kPLL_DIV_to_FLEXCOMM0,
    kPLL_DIV_to_FLEXCOMM1,
    kPLL_DIV_to_FLEXCOMM2,
    kPLL_DIV_to_FLEXCOMM3,
    kPLL_DIV_to_FLEXCOMM4,
    kPLL_DIV_to_FLEXCOMM5,
    kPLL_DIV_to_FLEXCOMM6,
    kPLL_DIV_to_FLEXCOMM7,
    kPLL_DIV_to_FLEXCOMM8,
    kPLL_DIV_to_FLEXCOMM9,
};

void MCX_BoardEarlyInit(void) {
    BOARD_InitBootClocks();
    BOARD_InitBootPins();
}

void *MCX_BoardGetUARTInstanceById(uint8_t id) {
    if(id < ARRAY_SIZE(s_mcxn947_uart_inst_map)) {
        return s_mcxn947_uart_inst_map[id];
    }

    /* No such UART */
    return NULL;
}

int MCX_BoardConfigureUARTClock(void *instance) {
    clock_attach_id_t attach_id = kNONE_to_NONE;
    uint8_t uart_id = 0;

    for(int i = 0; i < ARRAY_SIZE(s_mcxn947_uart_inst_map); i++) {
        if(s_mcxn947_uart_inst_map[i] == instance) {
            attach_id = s_mcxn947_flexcomm_clk_att_map[i];
            uart_id = i;
        }
    }

    /* No corresponding connection found */
    if(attach_id == kNONE_to_NONE) {
        return -1;
    }

    CLOCK_AttachClk(attach_id);

    return CLOCK_GetLPFlexCommClkFreq(uart_id);
}