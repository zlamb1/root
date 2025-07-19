#ifndef ROOT_SLAB_H
#define ROOT_SLAB_H 1

#include "types.h"

#define ROOT_SLAB_FLAG_SLAB_SLAB 0x1
#define ROOT_SLAB_FLAG_MALLOC    0x2

typedef struct root_node_t
{
  struct root_node_t *next;
} root_node_t;

typedef struct root_slab_t
{
  root_size_t sz;
  root_size_t cnt, cap;
  void *pages;
  root_node_t *freelist;
  struct root_slab_t *prev, *next;
  root_uint8_t flags;
} root_slab_t;

root_slab_t *root_create_slab (root_size_t sz, root_size_t align);
void *root_slab_alloc (root_slab_t *slab);
int root_slab_free (root_slab_t *slab, void *p, int release);
void root_slab_release (root_slab_t *slab);

#endif
