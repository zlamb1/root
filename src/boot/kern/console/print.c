#include <stdarg.h>

#include "console/print.h"
#include "types.h"

root_console_t *root_console;
root_fd_t *stdin, *stdout;

static char hdigits[16] = { '0', '1', '2', '3', '4', '5', '6', '7',
                            '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

typedef enum FMT_FLAGS
{
  FMT_FLAG_LEFT_JUSTIFY = 0x01,
  FMT_FLAG_FORCE_SIGN = 0x02,
  FMT_FLAG_SPACE_SIGN = 0x04,
  FMT_FLAG_PREFIX = 0x08,
  FMT_FLAG_PAD_ZERO = 0x10,
  FMT_FLAG_LONG = 0x20,
  FMT_FLAG_LONG_LONG = 0x40
} FMT_FLAGS;

void
root_initprint (root_console_t *con)
{
  root_console = con;
  stdin = &con->fd.fd;
  stdout = &con->fd.fd;
  console_clear (root_console, 0);
}

void
root_putchar (char ch)
{
  console_putchar_sync (root_console, ch);
}

root_u32
root_fprinti32 (root_fd_t *fd, root_i32 n, root_u8 base, root_u8 capital)
{
  root_u32 sign = 0;
  if (n < 0)
    {
      sign = 1;
      root_fputchar_unsynced (fd, '-');
      n = -n;
    }
  return sign + root_fprintu32 (fd, (root_u32) n, base, capital);
}

root_u32
root_fprinti64 (root_fd_t *fd, root_i64 n, root_u8 base, root_u8 capital)
{
  root_u32 sign = 0;
  if (n < 0)
    {
      sign = 1;
      root_fputchar_unsynced (fd, '-');
      n = -n;
    }
  return sign + root_fprintu64 (fd, (root_u64) n, base, capital);
}

root_u32
root_fprintu32 (root_fd_t *fd, root_u32 n, root_u8 base, root_u8 capital)
{
  if (n != 0)
    {
      root_u8 cnt = 0, tmp;
      root_u8 stack[32];
      while (n != 0)
        {
          stack[cnt++] = n % base;
          n /= base;
        }
      tmp = cnt;
      while (cnt)
        {
          char ch;
          root_u8 digit = stack[--cnt];
          if (digit < 10)
            ch = hdigits[digit];
          else
            ch = capital ? hdigits[digit] - 0x20 : hdigits[digit];
          root_fputchar_unsynced (fd, ch);
        }
      return tmp;
    }
  else
    {
      root_fputchar_unsynced (fd, '0');
      return 1;
    }
}

root_u32
root_fprintu64 (root_fd_t *fd, root_u64 n, root_u8 base, root_u8 capital)
{
  if (n != 0)
    {
      root_u8 cnt = 0, tmp;
      root_u8 stack[64];
      while (n != 0)
        {
          stack[cnt++] = n % base;
          n /= base;
        }
      tmp = cnt;
      while (cnt)
        {
          char ch;
          root_u8 digit = stack[--cnt];
          if (digit < 10)
            ch = hdigits[digit];
          else
            ch = capital ? hdigits[digit] - 0x20 : hdigits[digit];
          root_fputchar_unsynced (fd, ch);
        }
      return tmp;
    }
  else
    {
      root_fputchar_unsynced (fd, '0');
      return 1;
    }
}

static inline const char *
readflags (const char *fmt, FMT_FLAGS *flags)
{
  char c;
  *flags = 0;
readc:
  c = *fmt;
  switch (c)
    {
    case '-':
      *flags |= FMT_FLAG_LEFT_JUSTIFY;
      fmt++;
      goto readc;
    case '+':
      *flags |= FMT_FLAG_FORCE_SIGN;
      fmt++;
      goto readc;
    case ' ':
      *flags |= FMT_FLAG_SPACE_SIGN;
      fmt++;
      goto readc;
    case '#':
      *flags |= FMT_FLAG_PREFIX;
      fmt++;
      goto readc;
    case '0':
      *flags |= FMT_FLAG_PAD_ZERO;
      fmt++;
      goto readc;
    default:
      break;
    }
  return fmt;
}

static inline int
isdigit (char c)
{
  return c >= '0' && c <= '9';
}

static inline const char *
readwidth (const char *fmt, int *width)
{
  char c;
  int w = 0;
readc:
  c = *fmt;
  if (isdigit (c))
    {
      w = w * 10 + (c - '0');
      fmt++;
      goto readc;
    }
  else if (w == 0 && c == '*')
    {
      w = -1;
      fmt++;
    }
  *width = w;
  return fmt;
}

static inline const char *
readprecision (const char *fmt, int *precision)
{
  int p = 0;
  char c = *fmt;
  if (c == '.')
    {
      fmt++;
      if (*fmt == '*')
        {
          p = -1;
          fmt++;
          goto endread;
        }
    readc:
      c = *fmt;
      while (isdigit (c))
        {
          p = p * 10 + (c - '0');
          fmt++;
          goto readc;
        }
    }
  else
    p = -2;
endread:
  *precision = p;
  return fmt;
}

static int
printlen_none (root_fd_t *fd, int *len, va_list *args, int flags, int width,
               int precision, char c)
{
  (void) width;
  switch (c)
    {
    case 'i':
    case 'd':
      {
        root_i32 n = va_arg (*args, int);
        if (flags & FMT_FLAG_FORCE_SIGN && n > 0)
          {
            root_fputchar_unsynced (fd, '+');
            len++;
          }
        else if (flags & FMT_FLAG_SPACE_SIGN && n >= 0)
          {
            root_fputchar_unsynced (fd, ' ');
            len++;
          }
        len += root_fprinti32 (fd, n, 10, 0);
        break;
      }
    case 'u':
      {
        root_u32 n = va_arg (*args, unsigned int);
        if (flags & FMT_FLAG_FORCE_SIGN && n != 0)
          {
            root_fputchar_unsynced (fd, '+');
            len++;
          }
        else if (flags & FMT_FLAG_SPACE_SIGN)
          {
            root_fputchar_unsynced (fd, ' ');
            len++;
          }
        len += root_fprintu32 (fd, n, 10, 0);
        break;
      }
    case 'b':
    case 'B':
      {
        root_u32 n = va_arg (*args, unsigned int);
        if (flags & FMT_FLAG_PREFIX && n > 0)
          {
            root_fputchar_unsynced (fd, '0');
            root_fputchar_unsynced (fd, c);
            len += 2;
          }
        len += root_fprintu32 (fd, n, 2, 0);
        break;
      }
    case 'o':
      {
        root_u32 n = va_arg (*args, unsigned int);
        if (flags & FMT_FLAG_PREFIX && n > 0)
          {
            root_fputchar_unsynced (fd, '0');
            len++;
          }
        len += root_fprintu32 (fd, n, 8, 0);
        break;
      }
    case 'x':
    case 'X':
      {
        root_u32 n = va_arg (*args, unsigned int);
        if (flags & FMT_FLAG_PREFIX && n > 0)
          {
            root_fputchar_unsynced (fd, '0');
            root_fputchar_unsynced (fd, c);
            len += 2;
          }
        len += root_fprintu32 (fd, n, 16, c == 'X');
        break;
      }
    case 'c':
      root_fputchar_unsynced (fd, va_arg (*args, int));
      len++;
      break;
    case 's':
      {
        const char *s = (const char *) va_arg (*args, int);
        if (precision >= 0)
          {
            fd->write (fd, s, precision);
            len += precision;
          }
        else
          {
            while (*s != 0)
              {
                root_fputchar_unsynced (fd, *s++);
                len++;
              }
          }
        break;
      }
    case 'p':
      {
        uintptr_t p = (uintptr_t) (void *) va_arg (*args, int);
        root_fputchar_unsynced (fd, '0');
        root_fputchar_unsynced (fd, 'x');
        len += 2;
        len += root_fprintu32 (fd, (root_u32) p, 16, 0);
        break;
      }
    case 'n':
      {
        signed int *p = (signed int *) va_arg (*args, int);
        *p = *len;
        break;
      }
    case '%':
      root_fputchar_unsynced (fd, '%');
      len++;
      break;
    default:
      return -1;
    }
  return 0;
}

static int
printlen_longlong (root_fd_t *fd, int *len, va_list *args, int flags,
                   int width, int precision, char c)
{
  (void) width;
  (void) precision;
  switch (c)
    {
    case 'i':
    case 'd':
      {
        root_i64 n = va_arg (*args, long long int);
        if (flags & FMT_FLAG_FORCE_SIGN && n > 0)
          {
            root_fputchar_unsynced (fd, '+');
            len++;
          }
        else if (flags & FMT_FLAG_SPACE_SIGN && n >= 0)
          {
            root_fputchar_unsynced (fd, ' ');
            len++;
          }
        len += root_fprinti64 (fd, n, 10, 0);
        break;
      }
    case 'u':
      {
        root_u64 n = va_arg (*args, unsigned long long int);
        if (flags & FMT_FLAG_FORCE_SIGN && n != 0)
          {
            root_fputchar_unsynced (fd, '+');
            len++;
          }
        else if (flags & FMT_FLAG_SPACE_SIGN)
          {
            root_fputchar_unsynced (fd, ' ');
            len++;
          }
        len += root_fprintu64 (fd, n, 10, 0);
        break;
      }
    case 'b':
    case 'B':
      {
        root_u64 n = va_arg (*args, unsigned long long int);
        if (flags & FMT_FLAG_PREFIX && n > 0)
          {
            root_fputchar_unsynced (fd, '0');
            root_fputchar_unsynced (fd, c);
            len += 2;
          }
        len += root_fprintu64 (fd, n, 2, 0);
        break;
      }
    case 'o':
      {
        root_u64 n = va_arg (*args, unsigned long long int);
        if (flags & FMT_FLAG_PREFIX && n > 0)
          {
            root_fputchar_unsynced (fd, '0');
            len++;
          }
        len += root_fprintu64 (fd, n, 8, 0);
        break;
      }
    case 'x':
    case 'X':
      {
        root_u64 n = va_arg (*args, unsigned long long int);
        if (flags & FMT_FLAG_PREFIX && n > 0)
          {
            root_fputchar_unsynced (fd, '0');
            root_fputchar_unsynced (fd, c);
            len += 2;
          }
        len += root_fprintu64 (fd, n, 16, c == 'X');
        break;
      }
    case 'c':
    case 's':
    case 'p':
      return -1;
    case 'n':
      {
        long long int *p = (long long int *) va_arg (*args, long long int *);
        *p = *len;
        break;
      }
    case '%':
      root_fputchar_unsynced (fd, '%');
      len++;
      break;
    default:
      return -1;
    }
  return 0;
}

int
root_vfprintf (root_fd_t *fd, const char *fmt, va_list args)
{
  int len = 0;
  char ch;
readc:
  ch = *fmt++;
  if (ch == '%')
    {
      FMT_FLAGS flags;
      int width, precision, r;
      fmt = readflags (fmt, &flags);
      fmt = readwidth (fmt, &width);
      fmt = readprecision (fmt, &precision);
      if (width == -1)
        width = va_arg (args, int);
      if (precision == -1)
        precision = va_arg (args, int);
      ch = *fmt++;
      if (ch == 'l')
        {
          ch = *fmt++;
          if (ch != 'l')
            flags |= FMT_FLAG_LONG;
          else
            {
              ch = *fmt++;
              flags |= FMT_FLAG_LONG_LONG;
            }
        }
      if (flags & FMT_FLAG_LONG_LONG)
        r = printlen_longlong (fd, &len, &args, flags, width, precision, ch);
      else if (flags & FMT_FLAG_LONG)
        r = -1;
      else
        r = printlen_none (fd, &len, &args, flags, width, precision, ch);
      if (r)
        return r;
      goto readc;
    }
  else if (ch != '\0')
    {
      root_fputchar_unsynced (fd, ch);
      len++;
      goto readc;
    }
  root_cursor_sync ();
  return len;
}

typedef struct
{
  void *buf;
  root_fd_t base;
} fdmem_t;

int
root_sprintf (void *str, const char *fmt, ...)
{
  int len;
  va_list args;
  root_fdmem_t fd = fcreate_memfd (str, SIZE_MAX);
  char nul = '\0';
  va_start (args, fmt);
  len = root_vfprintf (&fd.fd, fmt, args);
  fd.fd.write (&fd.fd, &nul, 1);
  va_end (args);
  return len;
}
