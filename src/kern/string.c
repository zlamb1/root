#include "kern/string.h"
#include "kern/malloc.h"

#define ROOT_GENERIC_MEMMOVE

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

int
root_strncmp (const char *s1, const char *s2, root_size_t n)
{
  unsigned char ch1 = 0, ch2 = 0;
  for (root_size_t i = 0; i < n; i++)
    {
      ch1 = s1[i], ch2 = s2[i];
      if (ch1 == '\0' || ch1 != ch2)
        break;
    }
  return ch1 - ch2;
}

char *
root_strdup (const char *str)
{
  root_size_t len;
  char *s;
  if (str == NULL)
    return NULL;
  len = root_strlen (str);
  s = root_malloc (len + 1);
  if (s == NULL)
    return NULL;
  for (root_size_t i = 0; i < len; i++)
    s[i] = str[i];
  s[len] = '\0';
  return s;
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

#ifdef ROOT_GENERIC_MEMMOVE
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
#endif

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
