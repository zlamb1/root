#ifndef ROOT_MMAP_H
#define ROOT_MMAP_H 1

#include "types.h"

typedef enum root_mm_type_t
{
  ROOT_MM_TYPE_FREE = 0x1,
  ROOT_MM_TYPE_RESERVED = 0x2,
  ROOT_MM_TYPE_ACPI = 0x3,
  ROOT_MM_TYPE_ACPI_NVS = 0x4,
  ROOT_MM_TYPE_BAD = 0x5
} root_mm_type_t;

typedef struct root_mm_entry_t
{
  root_uint64_t base;
  root_uint64_t length;
  root_uint32_t type;
  root_uint32_t acpi_attribs;
} __attribute__ ((packed)) root_mm_entry_t;

typedef struct root_mmap_t
{
  root_uint32_t count;
  root_mm_entry_t *entries;
} root_mmap_t;

int root_init_mmap (root_mmap_t *mmap);

#endif
