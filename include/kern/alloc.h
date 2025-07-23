#ifndef ROOT_ALLOC_H
#define ROOT_ALLOC_H 1

#include "mmap.h"
#include "types.h"

int root_init_alloc (root_mmap_t *mmap);

void *root_alloc_page (void);
void *root_alloc_pages (root_uint32_t npages);

void root_free_pages (const void *p);

#endif
