#ifndef ROOT_ISR_H
#define ROOT_ISR_H 1

#include "types.h"

typedef struct root_idt_entry32_t
{
  root_u16 isr_low;
  root_u16 kernel_cs;
  root_u8 reserved;
  root_u8 attribs;
  root_u16 isr_high;
} __attribute__ ((packed)) root_idt_entry32_t;

typedef struct root_idtr_t
{
  root_u16 limit;
  root_u32 base;
} __attribute__ ((packed)) root_idtr_t;

void root_init_idt (void);
void root_load_idt (void);
void root_set_isr (root_u8 vector, void *isr, root_u8 attribs);

#endif
