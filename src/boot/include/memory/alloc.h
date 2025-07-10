#ifndef ROOT_ALLOC_H
#define ROOT_ALLOC_H 1

#include "types.h"

int root_initalloc (void);

void *root_alloc_page (void);
void *root_alloc_pages (root_uint32_t npages);

void root_free_pages (void *p);

#endif
