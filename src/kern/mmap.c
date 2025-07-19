#include "kern/mmap.h"
#include "kern/sort.h"
#include "kern/string.h"

#define MAX_MMAP_EVENTS (ROOT_MMAP_MAX_ENTRIES << 1)

typedef struct root_mm_entry_event_t
{
  root_mmap_entry_t *entry;
  root_uint64_t address;
} root_mm_entry_event_t;

static int
root_mmap_event_cmp (const void *e1, const void *e2)
{
  const root_mm_entry_event_t *evt1 = e1, *evt2 = e2;
  if (evt1->address != evt2->address)
    return evt1->address < evt2->address ? -1 : 1;
  return (evt1->address == evt1->entry->base)
         - (evt2->address == evt2->entry->base);
}

int
root_init_mmap (root_mmap_t *mmap)
{
  root_uint64_t last_addr = 0;
  root_uint32_t last_type = 0;
  root_uint16_t nentries = 0, nevts = 0, noverlap = 0;
  root_mmap_entry_t new_entries[ROOT_MMAP_MAX_ENTRIES];
  root_mm_entry_event_t events[MAX_MMAP_EVENTS];
  root_mmap_entry_t *overlap_list[ROOT_MMAP_MAX_ENTRIES];
  if (mmap == NULL || mmap->nentries <= 1)
    return ROOT_SUCCESS;
  for (int i = 0; i < mmap->nentries; i++)
    {
      root_mmap_entry_t *entry = mmap->entries + i;
      if (entry->base + entry->len <= entry->base)
        continue;
      events[nevts++]
          = (root_mm_entry_event_t){ .entry = entry, .address = entry->base };
      events[nevts++]
          = (root_mm_entry_event_t){ .entry = entry,
                                     .address = entry->base + entry->len };
    }
  root_ssort (events, nevts, sizeof (root_mm_entry_event_t),
              root_mmap_event_cmp);
  for (int i = 0; i < nevts; i++)
    {
      root_uint32_t cur_type = 0;
      root_mm_entry_event_t evt = events[i];
      root_mmap_entry_t *entry = evt.entry;
      if (evt.address == entry->base)
        overlap_list[noverlap++] = entry;
      else
        {
          for (int j = 0; j < noverlap; j++)
            {
              root_mmap_entry_t *cur_entry = overlap_list[j];
              if (cur_entry == entry)
                {
                  overlap_list[j] = overlap_list[noverlap - 1];
                  break;
                }
            }
          noverlap--;
        }
      for (int j = 0; j < noverlap; j++)
        {
          root_mmap_entry_t *cur_entry = overlap_list[j];
          if (cur_entry->type > cur_type)
            cur_type = cur_entry->type;
        }
      if (last_type != cur_type)
        {
          root_mmap_entry_t *new_entry = new_entries + nentries;
          if (last_type != 0)
            {
              new_entry->len = evt.address - last_addr;
              if (new_entry->len > 0 && ++nentries >= ROOT_MMAP_MAX_ENTRIES)
                return ROOT_ENOENT;
            }
          if (cur_type != 0)
            {
              new_entry->base = evt.address;
              new_entry->type = cur_type;
              last_addr = evt.address;
            }
          last_type = cur_type;
        }
    }
  root_memcpy (mmap->entries, new_entries,
               sizeof (root_mmap_entry_t) * nentries);
  mmap->nentries = nentries;
  return ROOT_SUCCESS;
}
