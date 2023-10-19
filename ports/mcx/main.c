#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "py/builtin.h"
#include "py/compile.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/stackctrl.h"
#include "py/mperrno.h"
#include "shared/readline/readline.h"
#include "shared/runtime/gchelper.h"
#include "shared/runtime/pyexec.h"

#include "extmod/vfs.h"

#include "drv_uart.h"

#include "board.h"

STATIC drv_uart_t s_stdio_uart_obj;

#if MICROPY_ENABLE_COMPILER
void do_str(const char *src, mp_parse_input_kind_t input_kind) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
        qstr source_name = lex->source_name;
        mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, true);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // uncaught exception
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}
#endif

extern uint32_t __StackTop, __StackLimit, __HeapBase, __HeapLimit;

int main(int argc, char **argv) {
    MCX_BoardEarlyInit();

    mp_stack_set_top(&__StackTop);
    mp_stack_set_limit((char *)&__StackTop - (char *)&__StackLimit - 1024);

    gc_init((void *)&__HeapBase, (void *)&__HeapLimit);

    mp_init();
    readline_init0();

    MP_STATE_PORT(stdio_uart) = &s_stdio_uart_obj;
    drv_uart_config_t uart_cfg = {
        .baud_rate = 115200,
        .data_bits = DRV_Uart8DataBits,
        .stop_bits = DRV_Uart1StopBits,
        .parity = DRV_UartNoParity,
    };

    drv_uart_init(&s_stdio_uart_obj, 4, &uart_cfg);

    pyexec_friendly_repl();



    mp_deinit();
    return 0;
}

void gc_collect(void) {
    gc_collect_start();
    gc_helper_collect_regs_and_stack();
    gc_collect_end();
    gc_dump_info(&mp_plat_print);
}

void nlr_jump_fail(void *val) {
    while (1) {
        ;
    }
}

void NORETURN __fatal_error(const char *msg) {
    while (1) {
        ;
    }
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
#endif
