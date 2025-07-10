#ifndef ROOT_BIOS_H
#define ROOT_BIOS_H 1

#include "types.h"

#define ROOT_BIOS_CARRY_FLAG  0x1
#define ROOT_BIOS_PARITY_FLAG 0x4

typedef struct root_bios_args_t
{
  root_uint32_t eax;
  root_uint32_t ebx;
  root_uint32_t ecx;
  root_uint32_t esi;
  root_uint32_t edi;
  root_uint32_t edx;
  root_uint16_t flags;
  root_uint16_t ds;
  root_uint16_t es;
  root_uint16_t pad[1];
} __attribute__ ((packed)) root_bios_args_t;

void root_bios_interrupt (root_uint8_t intnum, root_bios_args_t *args)
    __attribute__ ((regparm (3)));

static inline void *
root_get_pointer (root_uint16_t segment, root_uint16_t offset)
{
  return (void *) ((segment << 4) + offset);
}

static inline root_uint16_t
root_get_segment (root_uintptr_t address)
{
  return address >> 4;
}

static inline root_uint16_t
root_get_offset (root_uintptr_t address)
{
  return address % 16;
}

#endif
