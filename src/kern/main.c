#include "machine.h"
#include "print.h"

void __attribute__ ((noreturn)) root_main (void);

void
root_main (void)
{
  int len;
  root_machine_init ();
  len = root_printf ("%.1d\n", 0);
  for (;;)
    ;
}
