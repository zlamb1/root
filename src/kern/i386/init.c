#include "machine.h"
#include "vga.h"

void
root_machine_init (void)
{
  root_vga_term_mod_init ();
}
