#include "kern/ssp.h"
#include "kern/print.h"

uintptr_t __stack_chk_guard = 0xAD17F32C;

void
__stack_chk_fail (void)
{
  // TODO: panic
  root_printf ("stack smashing detected\n");
  for (;;)
    ;
}
