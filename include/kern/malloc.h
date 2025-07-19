#ifndef ROOT_MALLOC_H
#define ROOT_MALLOC_H 1

#include "types.h"

void *root_malloc (root_size_t sz);
void *root_realloc (void *p, root_size_t sz);
void root_free (void *p);

#endif
