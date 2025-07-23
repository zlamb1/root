#include "kern/machine.h"
#include "kern/mod.h"
#include "kern/print.h"
#include "mods/sh.h"

void __attribute__ ((noreturn)) root_main (void);

void
root_main (void)
{
  root_machine_init ();
  root_ok ("machine init");
  root_load_initmods ();
  root_shell_task ();
  for (;;)
    ;
}
