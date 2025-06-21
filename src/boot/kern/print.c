#include <stdarg.h>

#include "print.h"
#include "vga.h"

root_term_t *root_term;
static root_vga_term_t root_vga_term;

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

int
root_initprint (root_term_t *term)
{
  if (term != NULL)
    root_term = term;
  else
    {
      int r = root_initvga (&root_vga_term);
      if (r != 0)
        return r;
      root_term = &root_vga_term.base;
    }
  return 0;
}

root_u32
root_printi32_unsynced (root_i32 n, root_u8 base, root_u8 capital)
{
  root_u32 sign = 0;
  if (n < 0)
    {
      sign = 1;
      root_putchar ('-');
      n = -n;
    }
  return sign + root_printu32_unsynced ((root_u32) n, base, capital);
}

root_u32
root_printi64_unsynced (root_i64 n, root_u8 base, root_u8 capital)
{
  root_u32 sign = 0;
  if (n < 0)
    {
      sign = 1;
      root_putchar ('-');
      n = -n;
    }
  return sign + root_printu64_unsynced ((root_u64) n, base, capital);
}

root_u32
root_printu32_unsynced (root_u32 n, root_u8 base, root_u8 capital)
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
          root_u8 digit = stack[--cnt];
          if (digit < 10)
            root_putchar_unsynced (hdigits[digit]);
          else
            root_putchar_unsynced (capital ? hdigits[digit] - 0x20
                                           : hdigits[digit]);
        }
      return tmp;
    }
  else
    {
      root_putchar_unsynced ('0');
      return 1;
    }
}

root_u32
root_printu64_unsynced (root_u64 n, root_u8 base, root_u8 capital)
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
          root_u8 digit = stack[--cnt];
          if (digit < 10)
            root_putchar_unsynced (hdigits[digit]);
          else
            root_putchar_unsynced (capital ? hdigits[digit] - 0x20
                                           : hdigits[digit]);
        }
      return tmp;
    }
  else
    {
      root_putchar_unsynced ('0');
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
endread:
  *precision = p;
  return fmt;
}

static int
printlen_none (int *len, va_list *args, int flags, int width, int precision,
               char c)
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
            root_putchar_unsynced ('+');
            len++;
          }
        else if (flags & FMT_FLAG_SPACE_SIGN && n >= 0)
          {
            root_putchar_unsynced (' ');
            len++;
          }
        len += root_printi32_unsynced (n, 10, 0);
        break;
      }
    case 'u':
      {
        root_u32 n = va_arg (*args, unsigned int);
        if (flags & FMT_FLAG_FORCE_SIGN && n != 0)
          {
            root_putchar_unsynced ('+');
            len++;
          }
        else if (flags & FMT_FLAG_SPACE_SIGN)
          {
            root_putchar_unsynced (' ');
            len++;
          }
        len += root_printu32_unsynced (n, 10, 0);
        break;
      }
    case 'b':
    case 'B':
      {
        root_u32 n = va_arg (*args, unsigned int);
        if (flags & FMT_FLAG_PREFIX && n > 0)
          {
            root_putchar_unsynced ('0');
            root_putchar_unsynced (c);
            len += 2;
          }
        len += root_printu32_unsynced (n, 2, 0);
        break;
      }
    case 'o':
      {
        root_u32 n = va_arg (*args, unsigned int);
        if (flags & FMT_FLAG_PREFIX && n > 0)
          {
            root_putchar_unsynced ('0');
            len++;
          }
        len += root_printu32_unsynced (n, 8, 0);
        break;
      }
    case 'x':
    case 'X':
      {
        root_u32 n = va_arg (*args, unsigned int);
        if (flags & FMT_FLAG_PREFIX && n > 0)
          {
            root_putchar_unsynced ('0');
            root_putchar_unsynced (c);
            len += 2;
          }
        len += root_printu32_unsynced (n, 16, c == 'X');
        break;
      }
    case 'c':
      root_putchar_unsynced (va_arg (*args, int));
      len++;
      break;
    case 's':
      {
        const char *s = (const char *) va_arg (*args, int);
        if (precision > 0)
          {
            while (precision--)
              {
                root_putchar_unsynced (*s++);
                len++;
              }
          }
        else
          {
            while (*s != 0)
              {
                root_putchar_unsynced (*s++);
                len++;
              }
          }
        break;
      }
    case 'p':
      {
        uintptr_t p = (uintptr_t) (void *) va_arg (*args, int);
        root_putchar_unsynced ('0');
        root_putchar_unsynced ('x');
        len += 2;
        len += root_printu32_unsynced ((root_u32) p, 16, 0);
        break;
      }
    case 'n':
      {
        signed int *p = (signed int *) va_arg (*args, int);
        *p = *len;
        break;
      }
    case '%':
      root_putchar_unsynced ('%');
      len++;
      break;
    default:
      return -1;
    }
  return 0;
}

static int
printlen_longlong (int *len, va_list *args, int flags, int width,
                   int precision, char c)
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
            root_putchar_unsynced ('+');
            len++;
          }
        else if (flags & FMT_FLAG_SPACE_SIGN && n >= 0)
          {
            root_putchar_unsynced (' ');
            len++;
          }
        len += root_printi64_unsynced (n, 10, 0);
        break;
      }
    case 'u':
      {
        root_u64 n = va_arg (*args, unsigned long long int);
        if (flags & FMT_FLAG_FORCE_SIGN && n != 0)
          {
            root_putchar_unsynced ('+');
            len++;
          }
        else if (flags & FMT_FLAG_SPACE_SIGN)
          {
            root_putchar_unsynced (' ');
            len++;
          }
        len += root_printu64_unsynced (n, 10, 0);
        break;
      }
    case 'b':
    case 'B':
      {
        root_u64 n = va_arg (*args, unsigned long long int);
        if (flags & FMT_FLAG_PREFIX && n > 0)
          {
            root_putchar_unsynced ('0');
            root_putchar_unsynced (c);
            len += 2;
          }
        len += root_printu64_unsynced (n, 2, 0);
        break;
      }
    case 'o':
      {
        root_u64 n = va_arg (*args, unsigned long long int);
        if (flags & FMT_FLAG_PREFIX && n > 0)
          {
            root_putchar_unsynced ('0');
            len++;
          }
        len += root_printu64_unsynced (n, 8, 0);
        break;
      }
    case 'x':
    case 'X':
      {
        root_u64 n = va_arg (*args, unsigned long long int);
        if (flags & FMT_FLAG_PREFIX && n > 0)
          {
            root_putchar_unsynced ('0');
            root_putchar_unsynced (c);
            len += 2;
          }
        len += root_printu64_unsynced (n, 16, c == 'X');
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
      root_putchar_unsynced ('%');
      len++;
      break;
    default:
      return -1;
    }
  return 0;
}

int
root_printf (const char *fmt, ...)
{
  va_list args;
  int len = 0;
  char c;
  va_start (args, fmt);

readc:
  c = *fmt++;
  if (c == '%')
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
      c = *fmt++;
      if (c == 'l')
        {
          c = *fmt++;
          if (c != 'l')
            flags |= FMT_FLAG_LONG;
          else
            {
              c = *fmt++;
              flags |= FMT_FLAG_LONG_LONG;
            }
        }
      if (flags & FMT_FLAG_LONG_LONG)
        r = printlen_longlong (&len, &args, flags, width, precision, c);
      else if (flags & FMT_FLAG_LONG)
        r = -1;
      else
        r = printlen_none (&len, &args, flags, width, precision, c);
      if (r)
        return r;
      goto readc;
    }
  else if (c != '\0')
    {
      root_putchar_unsynced (c);
      len++;
      goto readc;
    }

  va_end (args);
  root_cursorsync ();
  return len;
}
