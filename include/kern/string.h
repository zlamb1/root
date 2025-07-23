#ifndef ROOT_STRING_H
#define ROOT_STRING_H 1

#include "types.h"

root_size_t root_strlen (const char *s);
int root_strcmp (const char *s1, const char *s2);
int root_strncmp (const char *s1, const char *s2, root_size_t n);

static int
root_strpre (const char *pre, const char *str)
{
  return root_strncmp (pre, str, root_strlen (pre)) == 0;
}

char *root_strdup (const char *str);

void root_memcpy (void *dst, const void *src, root_size_t n);
void root_memmove (void *dst, const void *src, root_size_t n);
void root_memset (void *dst, root_uint8_t v, root_size_t n);
void root_memsetw (void *dst, root_uint16_t v, root_size_t n);
void root_memsetdw (void *dst, root_uint32_t v, root_size_t n);

#endif
