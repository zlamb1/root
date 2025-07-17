#ifndef ROOT_I386PC_BIOS_H
#define ROOT_I386PC_BIOS_H 1

#include "types.h"

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

#endif
