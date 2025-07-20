#include "i386-pc/biosterm.h"
#include "i386-pc/e820.h"
#include "i386-pc/isr.h"
#include "i386-pc/pic.h"
#include "kern/alloc.h"
#include "kern/machine.h"
#include "kern/mmap.h"
#include "kern/print.h"

static root_mmap_t mmap = { 0 };

static void
init_alloc (void)
{
  if (root_mmap_e820 (&mmap))
    root_error ("failed to acquire e820 mmap\n");
  if (root_init_mmap (&mmap))
    root_error ("failed to init mmap\n");
  if (root_init_alloc (&mmap))
    root_error ("failed to init alloc\n");
}

void
root_machine_init (void)
{
  root_cli ();
  root_init_bios_term ();
  init_alloc ();
  root_init_idt ();
  root_load_idt ();
  root_pic_init (0x20, 0x28);
  root_sti ();
}
