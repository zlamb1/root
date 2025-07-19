#ifndef ROOT_MMAP_H
#define ROOT_MMAP_H 1

#include "types.h"

#ifndef ROOT_MMAP_MAX_ENTRIES
#define ROOT_MMAP_MAX_ENTRIES 64
#endif

typedef enum
{
  ROOT_MMAP_ENTRY_TYPE_FREE = 1,
  ROOT_MMAP_ENTRY_TYPE_RESERVED = 2,
  ROOT_MMAP_ENTRY_TYPE_ACPI = 3,
  ROOT_MMAP_ENTRY_TYPE_ACPI_NVS = 4,
  ROOT_MMAP_ENTRY_TYPE_BAD = 5,
} root_mmap_entry_type_t;

typedef struct
{
  root_uint64_t base, len;
  unsigned char type;
} root_mmap_entry_t;

typedef struct
{
  int nentries;
  root_mmap_entry_t entries[ROOT_MMAP_MAX_ENTRIES];
} root_mmap_t;

int root_init_mmap (root_mmap_t *mmap);

#endif
