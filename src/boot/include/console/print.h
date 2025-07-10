#ifndef ROOT_PRINT_H
#define ROOT_PRINT_H 1

#include <stdarg.h>

#include "console/console.h"
#include "io/fd.h"

extern root_console_t *root_console;
extern root_fd_t *stdin, *stdout;

void root_initprint (root_console_t *con);

root_uint32_t root_fprinti32 (root_fd_t *fd, root_int32_t n, root_uint8_t base,
                              root_uint8_t capital);
root_uint32_t root_fprinti64 (root_fd_t *fd, root_int64_t n, root_uint8_t base,
                              root_uint8_t capital);
root_uint32_t root_fprintu32 (root_fd_t *fd, root_uint32_t n,
                              root_uint8_t base, root_uint8_t capital);
root_uint32_t root_fprintu64 (root_fd_t *fd, root_uint64_t n,
                              root_uint8_t base, root_uint8_t capital);

inline static root_uint32_t root_printi32 (root_int32_t n, root_uint8_t base,
                                           root_uint8_t capital);
inline static root_uint32_t root_printi64 (root_int64_t n, root_uint8_t base,
                                           root_uint8_t capital);
inline static root_uint32_t root_printu32 (root_uint32_t n, root_uint8_t base,
                                           root_uint8_t capital);
inline static root_uint32_t root_printu64 (root_uint64_t n, root_uint8_t base,
                                           root_uint8_t capital);
inline static root_uint32_t
root_printi32_sync (root_int32_t n, root_uint8_t base, root_uint8_t capital);
inline static root_uint32_t
root_printi64_sync (root_int64_t n, root_uint8_t base, root_uint8_t capital);
inline static root_uint32_t
root_printu32_sync (root_uint32_t n, root_uint8_t base, root_uint8_t capital);
inline static root_uint32_t
root_printu64_sync (root_uint64_t n, root_uint8_t base, root_uint8_t capital);
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

inline static root_uint32_t
root_printi32 (root_int32_t n, root_uint8_t base, root_uint8_t capital)
{
  return root_fprinti32 (stdout, n, base, capital);
}

inline static root_uint32_t
root_printi64 (root_int64_t n, root_uint8_t base, root_uint8_t capital)
{
  return root_fprinti64 (stdout, n, base, capital);
}

inline static root_uint32_t
root_printu32 (root_uint32_t n, root_uint8_t base, root_uint8_t capital)
{
  return root_fprintu32 (stdout, n, base, capital);
}

inline static root_uint32_t
root_printu64 (root_uint64_t n, root_uint8_t base, root_uint8_t capital)
{
  return root_fprintu64 (stdout, n, base, capital);
}

root_uint32_t
root_printi32_sync (root_int32_t n, root_uint8_t base, root_uint8_t capital)
{
  root_uint32_t len = root_printi32 (n, base, capital);
  root_cursor_sync ();
  return len;
}

root_uint32_t
root_printi64_sync (root_int64_t n, root_uint8_t base, root_uint8_t capital)
{
  root_uint32_t len = root_printi64 (n, base, capital);
  root_cursor_sync ();
  return len;
}

root_uint32_t
root_printu32_sync (root_uint32_t n, root_uint8_t base, root_uint8_t capital)
{
  root_uint32_t len = root_printu32 (n, base, capital);
  root_cursor_sync ();
  return len;
}

root_uint32_t
root_printu64_sync (root_uint64_t n, root_uint8_t base, root_uint8_t capital)
{
  root_uint32_t len = root_printu64 (n, base, capital);
  root_cursor_sync ();
  return len;
}

int root_vfprintf (root_fd_t *fd, const char *fmt, va_list args);

int __attribute__ ((format (printf, 2, 3)))
root_sprintf (void *str, const char *fmt, ...);

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
