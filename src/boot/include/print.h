#ifndef ROOT_PRINT_H
#define ROOT_PRINT_H 1

#include "text.h"

extern root_term_t *root_term;

int root_initprint (root_term_t *term);

root_u32 root_printi32_unsynced (root_i32 n, root_u8 base, root_u8 capital);
root_u32 root_printi64_unsynced (root_i64 n, root_u8 base, root_u8 capital);
root_u32 root_printu32_unsynced (root_u32 n, root_u8 base, root_u8 capital);
root_u32 root_printu64_unsynced (root_u64 n, root_u8 base, root_u8 capital);

inline static root_u32 root_printi32 (root_i32 n, root_u8 base,
                                      root_u8 capital);
inline static root_u32 root_printi64 (root_i64 n, root_u8 base,
                                      root_u8 capital);
inline static root_u32 root_printu32 (root_u32 n, root_u8 base,
                                      root_u8 capital);
inline static root_u32 root_printu64 (root_u64 n, root_u8 base,
                                      root_u8 capital);
inline static void root_cursorsync (void);

inline static void
root_putchar (char c)
{
  root_term->putchar (root_term, c);
}

inline static void
root_putchar_unsynced (char c)
{
  root_term->putchar_unsynced (root_term, c);
}

void
root_cursorsync (void)
{
  root_term->setcursorpos (root_term, root_term->pos.x, root_term->pos.y);
}

root_u32
root_printi32 (root_i32 n, root_u8 base, root_u8 capital)
{
  root_u32 len = root_printi32_unsynced (n, base, capital);
  root_cursorsync ();
  return len;
}

root_u32
root_printi64 (root_i64 n, root_u8 base, root_u8 capital)
{
  root_u32 len = root_printi64_unsynced (n, base, capital);
  root_cursorsync ();
  return len;
}

root_u32
root_printu32 (root_u32 n, root_u8 base, root_u8 capital)
{
  root_u32 len = root_printu32_unsynced (n, base, capital);
  root_cursorsync ();
  return len;
}

root_u32
root_printu64 (root_u64 n, root_u8 base, root_u8 capital)
{
  root_u32 len = root_printu64_unsynced (n, base, capital);
  root_cursorsync ();
  return len;
}

int __attribute__ ((format (printf, 1, 2))) root_printf (const char *fmt, ...);

#endif
