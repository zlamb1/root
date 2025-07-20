#ifndef ROOT_ISR_H
#define ROOT_ISR_H 1

#include "kern/types.h"

typedef struct root_idt_entry_t
{
  root_uint16_t isr_low;
  root_uint16_t kernel_cs;
  root_uint8_t reserved;
  root_uint8_t attribs;
  root_uint16_t isr_high;
} __attribute__ ((packed)) root_idt_entry_t;

typedef struct root_idtr_t
{
  root_uint16_t limit;
  root_uint32_t base;
} __attribute__ ((packed)) root_idtr_t;

void root_init_idt (void);
void root_load_idt (void);
void root_set_isr (root_uint8_t vector, void *isr, root_uint8_t attribs);

#endif
