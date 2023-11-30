#include <stdlib.h>

#include "MCXN947_cm33_core0.h"
#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#include "pendsv.h"

static volatile void *pendsv_object;

#if defined(PENDSV_DISPATCH_NUM_SLOTS)
uint32_t pendsv_dispatch_active;
pendsv_dispatch_t pendsv_dispatch_table[PENDSV_DISPATCH_NUM_SLOTS];
#endif

void pendsv_init(void) {
#if defined(PENDSV_DISPATCH_NUM_SLOTS)
  pendsv_dispatch_active = false;
#endif

  NVIC_SetPriority(PendSV_IRQn, 7);
}

void pendsv_kbd_intr(void) {
  if(MP_STATE_MAIN_THREAD(mp_pending_exception) == MP_OBJ_NULL) {
    mp_sched_keyboard_interrupt();
  } else {
    MP_STATE_MAIN_THREAD(mp_pending_exception) = MP_OBJ_NULL;
    pendsv_object = &MP_STATE_VM(mp_kbd_exception);
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
  }
}

#if defined(PENDSV_DISPATCH_NUM_SLOTS)
void pendsv_schedule_dispatch(size_t slot, pendsv_dispatch_t f) {
  pendsv_dispatch_table[slot] = f;
  pendsv_dispatch_active = true;
  SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

void pendsv_dispatch_handler(void) {
  for (size_t i = 0; i < PENDSV_DISPATCH_NUM_SLOTS; ++i) {
    if (pendsv_dispatch_table[i] != NULL) {
      pendsv_dispatch_t f = pendsv_dispatch_table[i];
      pendsv_dispatch_table[i] = NULL;
      f();
    }
  }
}
#endif