#include "i386-pc/isr.h"
#include "kern/errno.h"
#include "kern/machine.h"
#include "kern/malloc.h"
#include "kern/print.h"

extern void *root_isr_stub_table[];

static root_idtr_t root_idtr;
static root_idt_entry_t *root_idt = NULL;

void __attribute__ ((noreturn)) exception_handler (int n);

void
exception_handler (int n)
{
  root_printf ("EXC HANDLER: %i\n", n);
  root_halt ();
}

void
root_init_idt (void)
{
  if (root_idt == NULL)
    {
      root_idt = root_malloc (sizeof (root_idt_entry_t) * 256);
      if (root_idt == NULL)
        {
          root_seterrno (ROOT_EALLOC);
          return;
        }
    }
  root_idtr.base = (root_uint32_t) root_idt;
  root_idtr.limit = sizeof (root_idt_entry_t) * 255;
  for (int i = 0; i < 32; i++)
    root_set_isr (i, root_isr_stub_table[i], 0x8E);
}

void
root_load_idt (void)
{
  if (root_idt == NULL)
    {
      root_seterrno (ROOT_EINVAL);
      return;
    }
  __asm__ volatile ("lidt %0" : : "m"(root_idtr));
}

void
root_set_isr (root_uint8_t vector, void *isr, root_uint8_t attribs)
{
  root_idt_entry_t *entry = &root_idt[vector];
  // TODO: disable interrupts to avoid race
  entry->isr_low = (root_uint32_t) isr & 0xFFFF;
  entry->kernel_cs = 0x8;
  entry->reserved = 0;
  entry->attribs = attribs;
  entry->isr_high = (root_uint32_t) isr >> 16;
}
