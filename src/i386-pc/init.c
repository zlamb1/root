#include "i386-pc/isr.h"
#include "i386-pc/pic.h"
#include "i386-pc/ps2.h"
#include "i386-pc/vga.h"
#include "kern/machine.h"

void
root_machine_init (void)
{
  root_cli ();
  root_vga_term_mod_init ();
  root_init_idt ();
  root_load_idt ();
  root_pic_init (0x20, 0x28);
  root_ps2_mod_init ();
  root_sti ();
}
