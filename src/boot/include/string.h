#ifndef ROOT_STRING_H
#define ROOT_STRING_H 1

#include "types.h"

root_size_t root_strlen (const char *s);
int root_strcmp (const char *s1, const char *s2);

void root_memcpy (void *dst, const void *src, root_size_t n);
void root_memmove (void *dst, const void *src, root_size_t n);
void root_memset (void *src, root_u8 v, root_size_t n);
void root_memsetw (void *src, root_u16 v, root_size_t n);
void root_memsetdw (void *dst, root_u32 v, root_size_t n);

#endif
