#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "py/builtin.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "shared/runtime/pyexec.h"

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

static char *stack_top;
#if MICROPY_ENABLE_GC
static char heap[MICROPY_HEAP_SIZE];
#endif

int main(int argc, char **argv) {
    int stack_dummy;
    stack_top = (char *)&stack_dummy;

    #if MICROPY_ENABLE_GC
    gc_init(heap, heap + sizeof(heap));
    #endif
    mp_init();
    #if MICROPY_ENABLE_COMPILER
    #if MICROPY_REPL_EVENT_DRIVEN
    pyexec_event_repl_init();
    for (;;) {
        int c = mp_hal_stdin_rx_chr();
        if (pyexec_event_repl_process_char(c)) {
            break;
        }
    }
    #else
    pyexec_friendly_repl();
    #endif
    // do_str("print('hello world!', list(x+1 for x in range(10)), end='eol\\n')", MP_PARSE_SINGLE_INPUT);
    // do_str("for i in range(10):\r\n  print(i)", MP_PARSE_FILE_INPUT);
    #else
    pyexec_frozen_module("frozentest.py", false);
    #endif
    mp_deinit();
    return 0;
}

#if MICROPY_ENABLE_GC
void gc_collect(void) {
    // WARNING: This gc_collect implementation doesn't try to get root
    // pointers from CPU registers, and thus may function incorrectly.
    void *dummy;
    gc_collect_start();
    gc_collect_root(&dummy, ((mp_uint_t)stack_top - (mp_uint_t)&dummy) / sizeof(mp_uint_t));
    gc_collect_end();
    gc_dump_info(&mp_plat_print);
}
#endif

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    mp_raise_OSError(MP_ENOENT);
}

mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
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

// this is a minimal IRQ and reset framework for any Cortex-M CPU

extern uint32_t __StackTop, __etext, __data_start__, __data_end__, __bss_start__, __bss_end__;

void Reset_Handler(void) __attribute__((naked));
void Reset_Handler(void) {
    // set stack pointer
    __asm volatile ("ldr sp, =__StackTop");
    // copy .data section from flash to RAM
    for (uint32_t *src = &__etext, *dest = &__data_start__; dest < &__data_end__;) {
        *dest++ = *src++;
    }
    // zero out .bss section
    for (uint32_t *dest = &__bss_start__; dest < &__bss_end__;) {
        *dest++ = 0;
    }
    // jump to board initialisation
    void _start(void);
    _start();
}

void Default_Handler(void) {
    for (;;) {
    }
}

const uint32_t isr_vector[] __attribute__((section(".isr_vector"))) = {
    (uint32_t)&__StackTop,
    (uint32_t)&Reset_Handler,
    (uint32_t)&Default_Handler, // NMI_Handler
    (uint32_t)&Default_Handler, // HardFault_Handler
    (uint32_t)&Default_Handler, // MemManage_Handler
    (uint32_t)&Default_Handler, // BusFault_Handler
    (uint32_t)&Default_Handler, // UsageFault_Handler
    0,
    0,
    0,
    0,
    (uint32_t)&Default_Handler, // SVC_Handler
    (uint32_t)&Default_Handler, // DebugMon_Handler
    0,
    (uint32_t)&Default_Handler, // PendSV_Handler
    (uint32_t)&Default_Handler, // SysTick_Handler
};

void _start(void) {
    // when we get here: stack is initialised, bss is clear, data is copied

    // SCB->CCR: enable 8-byte stack alignment for IRQ handlers, in accord with EABI
    *((volatile uint32_t *)0xe000ed14) |= 1 << 9;

    // Do Board_InitBootxxx here.

    // now that we have a basic system up and running we can call main
    main(0, NULL);

    // we must not return
    for (;;) {
    }
}

