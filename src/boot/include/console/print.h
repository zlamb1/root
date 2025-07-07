#ifndef ROOT_PRINT_H
#define ROOT_PRINT_H 1

#include <stdarg.h>

#include "console/console.h"
#include "io/fd.h"

extern root_console_t *root_console;
extern root_fd_t *stdin, *stdout;

void root_initprint (root_console_t *con);

root_u32 root_fprinti32 (root_fd_t *fd, root_i32 n, root_u8 base,
                         root_u8 capital);
root_u32 root_fprinti64 (root_fd_t *fd, root_i64 n, root_u8 base,
                         root_u8 capital);
root_u32 root_fprintu32 (root_fd_t *fd, root_u32 n, root_u8 base,
                         root_u8 capital);
root_u32 root_fprintu64 (root_fd_t *fd, root_u64 n, root_u8 base,
                         root_u8 capital);

inline static root_u32 root_printi32 (root_i32 n, root_u8 base,
                                      root_u8 capital);
inline static root_u32 root_printi64 (root_i64 n, root_u8 base,
                                      root_u8 capital);
inline static root_u32 root_printu32 (root_u32 n, root_u8 base,
                                      root_u8 capital);
inline static root_u32 root_printu64 (root_u64 n, root_u8 base,
                                      root_u8 capital);
inline static root_u32 root_printi32_sync (root_i32 n, root_u8 base,
                                           root_u8 capital);
inline static root_u32 root_printi64_sync (root_i64 n, root_u8 base,
                                           root_u8 capital);
inline static root_u32 root_printu32_sync (root_u32 n, root_u8 base,
                                           root_u8 capital);
inline static root_u32 root_printu64_sync (root_u64 n, root_u8 base,
                                           root_u8 capital);
inline static void root_cursor_sync (void);

void root_putchar (char ch);

inline static void
root_fputchar_unsynced (root_fd_t *fd, char ch)
{
  fd->write (fd, &ch, 1);
}

inline static void
root_putchar_unsynced (char ch)
{
  root_console->putchar (root_console, ch);
}

void
root_cursor_sync (void)
{
  root_console->putcursor (root_console, root_console->x, root_console->y);
}

inline static root_u32
root_printi32 (root_i32 n, root_u8 base, root_u8 capital)
{
  return root_fprinti32 (stdout, n, base, capital);
}

inline static root_u32
root_printi64 (root_i64 n, root_u8 base, root_u8 capital)
{
  return root_fprinti64 (stdout, n, base, capital);
}

inline static root_u32
root_printu32 (root_u32 n, root_u8 base, root_u8 capital)
{
  return root_fprintu32 (stdout, n, base, capital);
}

inline static root_u32
root_printu64 (root_u64 n, root_u8 base, root_u8 capital)
{
  return root_fprintu64 (stdout, n, base, capital);
}

root_u32
root_printi32_sync (root_i32 n, root_u8 base, root_u8 capital)
{
  root_u32 len = root_printi32 (n, base, capital);
  root_cursor_sync ();
  return len;
}

root_u32
root_printi64_sync (root_i64 n, root_u8 base, root_u8 capital)
{
  root_u32 len = root_printi64 (n, base, capital);
  root_cursor_sync ();
  return len;
}

root_u32
root_printu32_sync (root_u32 n, root_u8 base, root_u8 capital)
{
  root_u32 len = root_printu32 (n, base, capital);
  root_cursor_sync ();
  return len;
}

root_u32
root_printu64_sync (root_u64 n, root_u8 base, root_u8 capital)
{
  root_u32 len = root_printu64 (n, base, capital);
  root_cursor_sync ();
  return len;
}

int root_vfprintf (root_fd_t *fd, const char *fmt, va_list args);

static inline int __attribute__ ((format (printf, 1, 2)))
root_printf (const char *fmt, ...)
{
  int len;
  va_list args;
  va_start (args, fmt);
  len = root_vfprintf (stdout, fmt, args);
  va_end (args);
  return len;
}

#endif
