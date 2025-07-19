#ifndef ROOT_SORT_H
#define ROOT_SORT_H 1

#include "types.h"

void root_ssort (void *arr, root_size_t nelements, root_size_t elsize,
                 int (*cmp) (const void *, const void *));

#endif
