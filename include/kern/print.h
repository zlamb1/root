#ifndef ROOT_PRINT_H
#define ROOT_PRINT_H 1

#include <stdarg.h>

#include "fd.h"
#include "machine.h"
#include "malloc.h"
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

static __attribute__ ((format (printf, 3, 4))) int
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
root_vprintf (const char *fmt, va_list args)
{
  int len;
  char buf[ROOT_PRINT_PREALLOC];
  len = root_vsnprintf (buf, ROOT_PRINT_PREALLOC, fmt, args);
  if (len >= ROOT_PRINT_PREALLOC)
    {
      char *tmp = root_malloc (len + 1);
      if (tmp == NULL)
        return ROOT_EALLOC;
      len = root_vsnprintf (tmp, len + 1, fmt, args);
      if (len > 0)
        {
          root_write (ROOT_STDOUT, tmp, len);
          root_cursorsync ();
        }
      root_free (tmp);
      return len;
    }
  else if (len > 0)
    {
      root_write (ROOT_STDOUT, buf, len);
      root_cursorsync ();
    }
  return len;
}

static __attribute__ ((format (printf, 1, 2))) int
root_printf (const char *fmt, ...)
{
  int len;
  char buf[ROOT_PRINT_PREALLOC];
  va_list args;
  va_start (args, fmt);
  len = root_vprintf (fmt, args);
  va_end (args);
  return len;
}

static __attribute__ ((format (printf, 1, 2))) void
root_ok (const char *fmt, ...)
{
  int len;
  va_list args;
  char buf[ROOT_PRINT_PREALLOC];
  root_printf ("\033[92m[ OK ] \033[0m");
  va_start (args, fmt);
  root_vprintf (fmt, args);
  va_end (args);
  root_printf ("\n");
}

static __attribute__ ((format (printf, 1, 2))) void
root_warn (const char *fmt, ...)
{
  int len;
  va_list args;
  char buf[ROOT_PRINT_PREALLOC];
  root_printf ("\033[93m[ WARN ] \033[0m");
  va_start (args, fmt);
  root_vprintf (fmt, args);
  va_end (args);
  root_printf ("\n");
}

static __attribute__ ((format (printf, 1, 2))) void
root_error (const char *fmt, ...)
{
  int len;
  va_list args;
  char buf[ROOT_PRINT_PREALLOC];
  root_printf ("\033[31m[ ERROR ] \033[0m");
  va_start (args, fmt);
  root_vprintf (fmt, args);
  va_end (args);
  root_printf ("\n");
  root_halt ();
}

#endif
