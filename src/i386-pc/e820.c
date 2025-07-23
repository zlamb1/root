#include "i386-pc/e820.h"
#include "i386-pc/bios.h"
#include "kern/mmap.h"
#include "kern/string.h"

typedef struct
{
  root_uint64_t base, len;
  root_uint32_t type, attribs;
} __attribute__ ((packed)) root_e820_entry_t;

static int
root_get_e820_entry (root_bios_args_t *args, root_e820_entry_t *entry)
{
  root_memset (entry, 0, sizeof (root_e820_entry_t));
  args->eax = 0xE820;
  args->ecx = 20;
  args->edx = 0x534D4150;
  args->es = root_get_real_segment (entry);
  args->edi = root_get_real_offset (entry);
  root_bios_interrupt (0x15, args);
  if (args->flags & ROOT_BIOS_CARRY_FLAG)
    return -1;
  switch (entry->type)
    {
    case ROOT_MMAP_ENTRY_TYPE_FREE:
    case ROOT_MMAP_ENTRY_TYPE_RESERVED:
    case ROOT_MMAP_ENTRY_TYPE_ACPI:
    case ROOT_MMAP_ENTRY_TYPE_ACPI_NVS:
    case ROOT_MMAP_ENTRY_TYPE_BAD:
      break;
    default:
      entry->type = ROOT_MMAP_ENTRY_TYPE_RESERVED;
      break;
    }
  return ROOT_SUCCESS;
}

int
root_mmap_e820 (root_mmap_t *mmap)
{
  root_bios_args_t args;
  root_mmap_entry_t *mmap_entry;
  root_e820_entry_t entry;
  int r;
  if (mmap == NULL)
    return ROOT_EINVAL;
  mmap->nentries = 0;
  args.ebx = 0;
  do
    {
      r = root_get_e820_entry (&args, &entry);
      if (r)
        return r;
      mmap_entry = mmap->entries + mmap->nentries;
      mmap_entry->base = entry.base;
      mmap_entry->len = entry.len;
      mmap_entry->type = entry.type;
      if (++mmap->nentries >= ROOT_MMAP_MAX_ENTRIES)
        break;
    }
  while (args.eax == 0x534D4150 && args.ebx != 0);
  return ROOT_SUCCESS;
}
