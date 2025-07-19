#include "kern/machine.h"
#ifndef ROOT_PRINT_H
#define ROOT_PRINT_H 1

#include <stdarg.h>

#include "fd.h"
#include "types.h"

#define ROOT_STDIN  0
#define ROOT_STDOUT 1

#define ROOT_PRINT_PREALLOC 128

#define root_cursorsync()  root_fcursorsync (ROOT_STDOUT)
#define root_putchar(ch)   root_fputchar (ROOT_STDOUT, (ch))
#define root_putchar_u(ch) root_fputchar_u (ROOT_STDOUT, (ch))

void root_fcursorsync (int fd);

void root_fputchar (int fd, char ch);
void root_fputchar_u (int fd, char ch);

int root_vsnprintf (char *buf, root_size_t size, const char *fmt,
                    va_list args);

static int
root_snprintf (char *buf, root_size_t size, const char *fmt, ...)
{
  int len;
  va_list args;
  va_start (args, fmt);
  len = root_vsnprintf (buf, size, fmt, args);
  va_end (args);
  return len;
}

static int
root_printf (const char *fmt, ...)
{
  int len;
  char buf[ROOT_PRINT_PREALLOC];
  va_list args;
  va_start (args, fmt);
  len = root_vsnprintf (buf, ROOT_PRINT_PREALLOC, fmt, args);
  root_write (ROOT_STDOUT, buf, len);
  root_cursorsync ();
  va_end (args);
  return len;
}

static void
root_error (const char *fmt, ...)
{
  (void) fmt;
  root_halt ();
}

#endif
