#ifndef ROOT_MACHINE_IO_H
#define ROOT_MACHINE_IO_H 1

#include "types.h"

static inline root_u8
root_inb (root_u16 port)
{
  root_u8 val;
  __asm__ volatile ("inb %w1, %b0" : "=a"(val) : "Nd"(port) : "memory");
  return val;
}

static inline void
root_outb (root_u16 port, root_u8 val)
{
  __asm__ volatile ("outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

#endif
