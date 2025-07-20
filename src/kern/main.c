#include "kern/input.h"
#include "kern/machine.h"
#include "kern/mod.h"
#include "kern/print.h"

void __attribute__ ((noreturn)) root_main (void);

void
root_main (void)
{
  root_machine_init ();
  root_ok ("machine init");
  root_initmods ();
  for (;;)
    {
      root_input_event_t evt;
      if (root_poll_input (&evt) && evt.state != ROOT_KEY_RELEASE && evt.ascii)
        root_printf ("%c", evt.ascii);
    }
}
