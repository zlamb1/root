#ifndef ROOT_PAGE_H
#define ROOT_PAGE_H 1

#include "kern/types.h"

#define ROOT_PAGE_SIZE 4096
#define ROOT_PAGE_POW  12

#define ROOT_PAGE_ALIGN_DOWN(X) ((X) & ~(ROOT_PAGE_SIZE - 1))
#define ROOT_PAGE_ALIGN_UP(X)                                                 \
  ((((X) + (ROOT_PAGE_SIZE - 1)) & ~(ROOT_PAGE_SIZE - 1)))

#define ROOT_PG_FLAG_FREE 0x1
#define ROOT_PG_FLAG_HEAD 0x2
#define ROOT_PG_FLAG_TAIL 0x4
#define ROOT_PG_FLAG_SLAB 0x8

struct root_slab_t;

typedef struct root_page_t
{
  root_uint32_t flags;
  struct root_slab_t *slab;
  root_uint32_t npages;
} root_page_t;

typedef struct root_page_map_t
{
  root_size_t num_pages, num_upages;
  root_page_t *pages;
} root_page_map_t;

extern root_page_map_t root_page_map;

static root_page_t *
root_get_page (const void *p)
{
  root_uintptr_t addr = (root_uintptr_t) p;
  root_uintptr_t index = addr >> ROOT_PAGE_POW;
  if (index >= root_page_map.num_pages)
    return NULL;
  return root_page_map.pages + index;
}

#endif
