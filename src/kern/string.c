#include "string.h"

root_size_t
root_strlen (const char *s)
{
  root_size_t len = 0;
  while (s[len])
    len++;
  return len;
}

int
root_strcmp (const char *s1, const char *s2)
{
  unsigned char ch1 = *s1, ch2 = *s2;
  while (ch1 != '\0' && ch1 == ch2)
    {
      ch1 = *++s1;
      ch2 = *++s2;
    }
  return ch1 - ch2;
}

#ifdef ROOT_GENERIC_MEMCPY
void
root_memcpy (void *dst, const void *src, root_size_t n)
{
  unsigned char *d = (unsigned char *) dst;
  const unsigned char *s = (const unsigned char *) src;
  while (n--)
    *d++ = *s++;
}
#endif

void
root_memmove (void *dst, const void *src, root_size_t n)
{
  unsigned char *d = (unsigned char *) dst;
  const unsigned char *s = (const unsigned char *) src;
  if ((root_uintptr_t) src < (root_uintptr_t) dst)
    {
      while (n--)
        d[n] = s[n];
    }
  else
    {
      while (n--)
        *d++ = *s++;
    }
}

#ifdef ROOT_GENERIC_MEMSET
void
root_memset (void *src, root_u8 v, size_t n)
{
  root_u8 *s = (root_u8 *) src;
  while (n--)
    *s++ = v;
}
#endif

#ifdef ROOT_GENERIC_MEMSETW
void
root_memsetw (void *src, root_u16 v, size_t n)
{
  root_u16 *s = (root_u16 *) src;
  while (n--)
    *s++ = v;
}
#endif

#ifdef ROOT_GENERIC_MEMSETDW
void
root_memsetdw (void *dst, root_u32 v, root_size_t n)
{
  root_u32 *d = (root_u32 *) dst;
  while (n--)
    *d++ = v;
}
#endif
