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
  if (len > 0)
    {
      if (len >= ROOT_PRINT_PREALLOC)
        {
          char *tmp = root_malloc (len + 1);
          len = root_vsnprintf (tmp, len + 1, fmt, args);
          if (len > 0)
            {
              root_write (ROOT_STDOUT, tmp, len);
              root_cursorsync ();
            }
          root_free (tmp);
          return len;
        }
      else
        {
          root_write (ROOT_STDOUT, buf, len);
          root_cursorsync ();
        }
    }
  va_end (args);
  return len;
}

static void
root_ok (const char *fmt, ...)
{
  int len;
  va_list args;
  char buf[ROOT_PRINT_PREALLOC];
  root_printf ("\033[32m[ OK ] \033[0m");
  va_start (args, fmt);
  if ((len = root_vsnprintf (buf, ROOT_PRINT_PREALLOC, fmt, args)) > 0)
    root_write (ROOT_STDOUT, buf, len);
  root_printf ("\n");
  va_end (args);
}

static void
root_warn (const char *fmt, ...)
{
  int len;
  va_list args;
  char buf[ROOT_PRINT_PREALLOC];
  root_printf ("\033[93m[ WARN ] \033[0m");
  va_start (args, fmt);
  if ((len = root_vsnprintf (buf, ROOT_PRINT_PREALLOC, fmt, args)) > 0)
    root_write (ROOT_STDOUT, buf, len);
  root_printf ("\n");
  va_end (args);
}

static void
root_error (const char *fmt, ...)
{
  int len;
  va_list args;
  char buf[ROOT_PRINT_PREALLOC];
  root_printf ("\033[31m[ ERROR ] \033[0m");
  va_start (args, fmt);
  if ((len = root_vsnprintf (buf, ROOT_PRINT_PREALLOC, fmt, args)) > 0)
    root_write (ROOT_STDOUT, buf, len);
  root_printf ("\n");
  va_end (args);
  root_halt ();
}

#endif
