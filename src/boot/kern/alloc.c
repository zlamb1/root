#include "alloc.h"
#include "mmap.h"
#include "print.h"
#include "types.h"

static root_mmap_t root_mmap;

typedef struct root_page_t
{
  struct root_page_t *next;
} root_page_t;

root_page_t *root_pages = NULL;

int
root_initalloc (void)
{
  int r = root_init_mmap (&root_mmap);
  if (r)
    return r;
  for (root_size_t i = 0; i < root_mmap.count; i++)
    {
      root_mm_entry_t *entry = root_mmap.entries + i;
      if (entry->type == ROOT_MM_TYPE_FREE && entry->base >= 0x100000)
        {
          root_uintptr_t address = ((entry->base + 4095) >> 12) << 12;
          root_u64 right = entry->base + entry->length;
          while (address <= right - 0x1000)
            {
              root_page_t *page = (root_page_t *) address;
              page->next = root_pages;
              root_pages = page;
              address += 0x1000;
            }
        }
    }
  return 0;
}

void *
root_alloc_page (void)
{
  root_page_t *page;
  if (root_pages == NULL)
    return NULL;
  page = root_pages;
  root_pages = root_pages->next;
  return (void *) page;
}

void
root_free_page (void *p)
{
  root_page_t *page = (root_page_t *) p;
  if (((root_uintptr_t) page % 0x1000) != 0)
    {
      // TODO: panic!
      root_printf ("attempted to free non-page aligned pointer : %#X\n",
                   (unsigned int) (root_uintptr_t) page);
      for (;;)
        ;
    }
  page->next = root_pages;
  root_pages = page;
}
