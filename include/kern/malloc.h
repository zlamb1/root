#ifndef ROOT_MALLOC_H
#define ROOT_MALLOC_H 1

#include "string.h"
#include "types.h"

void *root_malloc (root_size_t sz);

static void *
root_zalloc (root_size_t sz)
{
  void *p = root_malloc (sz);
  if (p == NULL)
    return NULL;
  root_memset (p, 0, sz);
  return p;
}

void *root_realloc (const void *p, root_size_t sz);
void root_free (const void *p);

#endif
