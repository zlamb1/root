#ifndef ROOT_BIOS_H
#define ROOT_BIOS_H 1

#include "types.h"

#define ROOT_BIOS_CARRY_FLAG  0x1
#define ROOT_BIOS_PARITY_FLAG 0x4

typedef struct root_bios_args_t
{
  root_u32 eax;
  root_u32 ebx;
  root_u32 ecx;
  root_u32 esi;
  root_u32 edi;
  root_u32 edx;
  root_u16 flags;
  root_u16 ds;
  root_u16 es;
  root_u16 pad[1];
} __attribute__ ((packed)) root_bios_args_t;

void root_bios_interrupt (root_u8 intnum, root_bios_args_t *args)
    __attribute__ ((regparm (3)));

static inline void *
root_get_pointer (root_u16 segment, root_u16 offset)
{
  return (void *) ((segment << 4) + offset);
}

static inline root_u16
root_get_segment (root_uintptr_t address)
{
  return address >> 4;
}

static inline root_u16
root_get_offset (root_uintptr_t address)
{
  return address % 16;
}

#endif
