#include "board.h"

#include "clock_config.h"
#include "pin_mux.h"

typedef struct mcxn947_irq_table_type {
    mcx_board_isr_t irq_uart[10];
    void *          irq_uart_params[10];
    mcx_board_isr_t irq_ostimer[1];
    void *          irq_ostimer_params[1];
} mcxn947_irq_table_t;

STATIC volatile mcxn947_irq_table_t s_mcxn947_irq_table;

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

static OSTIMER_Type * const s_mcxn947_ostimer_inst_map[] = {
    OSTIMER0,
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

static clock_attach_id_t const s_mcxn947_ostimer_clk_att_map[] = {
    kCLK_1M_to_OSTIMER,
};

void MCX_BoardEarlyInit(void) {
    BOARD_InitBootClocks();
    BOARD_InitBootPins();
}

void *MCX_BoardGetUARTInstance(uint8_t id) {
    if(id < ARRAY_SIZE(s_mcxn947_uart_inst_map)) {
        return s_mcxn947_uart_inst_map[id];
    }

    /* No such UART */
    return NULL;
}

int MCX_BoardConfigureUARTClock(uint8_t id) {
    clock_attach_id_t attach_id = kNONE_to_NONE;

    if(id >= ARRAY_SIZE(s_mcxn947_flexcomm_clk_att_map)) {
        return -1;
    }

    attach_id = s_mcxn947_flexcomm_clk_att_map[id];

    CLOCK_AttachClk(attach_id);

    return CLOCK_GetLPFlexCommClkFreq(id);
}

int MCX_BoardConfigureUARTISR(uint8_t id, mcx_board_isr_t isr, void *param) {
    if(id >= ARRAY_SIZE(s_mcxn947_irq_table.irq_uart)) {
        return -1;
    }

    s_mcxn947_irq_table.irq_uart[id] = isr;
    s_mcxn947_irq_table.irq_uart_params[id] = param;

    return 0;
}

void MCX_BoardGenericUARTISR(uint8_t id) {
    if(id >= ARRAY_SIZE(s_mcxn947_irq_table.irq_uart)) {
        return;
    }

    mcx_board_isr_t isr = s_mcxn947_irq_table.irq_uart[id];

    if(isr == NULL) {
        return;
    }

    isr(s_mcxn947_irq_table.irq_uart_params[id]);
}

void *MCX_BoardGetOSTimerInstance(uint8_t id) {
    if(id < ARRAY_SIZE(s_mcxn947_ostimer_inst_map)) {
        return s_mcxn947_ostimer_inst_map[id];
    }

    /* No such OSTimer */
    return NULL;
}

int MCX_BoardConfigureOSTimerClock(uint8_t id) {
    clock_attach_id_t attach_id = kNONE_to_NONE;

    if(id >= ARRAY_SIZE(s_mcxn947_ostimer_clk_att_map)) {
        return -1;
    }

    attach_id = s_mcxn947_ostimer_clk_att_map[id];

    CLOCK_AttachClk(attach_id);

    return CLOCK_GetOstimerClkFreq();
}

int MCX_BoardConfigureOSTimerISR(uint8_t id, mcx_board_isr_t isr, void *param) {
    if(id >= ARRAY_SIZE(s_mcxn947_irq_table.irq_ostimer)) {
        return -1;
    }

    s_mcxn947_irq_table.irq_ostimer[id] = isr;
    s_mcxn947_irq_table.irq_ostimer_params[id] = param;

    return 0;
}

void MCX_BoardGenericOSTimerISR(uint8_t id) {
    if(id >= ARRAY_SIZE(s_mcxn947_irq_table.irq_ostimer)) {
        return;
    }

    mcx_board_isr_t isr = s_mcxn947_irq_table.irq_ostimer[id];

    if(isr == NULL) {
        return;
    }

    isr(s_mcxn947_irq_table.irq_ostimer_params[id]);
}