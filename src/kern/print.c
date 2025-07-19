#include "kern/print.h"
#include "kern/fd.h"
#include "kern/string.h"
#include "kern/term.h"
#include "kern/types.h"

#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#define MIN(X, Y) ((X) > (Y) ? (Y) : (X))

typedef enum
{
  PRINTF_FLAG_LEFT_JUSTIFY = 0x1,
  PRINTF_FLAG_FORCE_SIGN = 0x2,
  PRINTF_FLAG_BLANK_SPACE = 0x4,
  PRINTF_FLAG_PREFIX = 0x8,
  PRINTF_FLAG_LPAD_ZERO = 0x10
} printf_flags;

typedef enum
{
  PRINTF_LENGTH_NONE,
  /* save space by referencing default row */
  PRINTF_LENGTH_HALF = PRINTF_LENGTH_NONE,
  PRINTF_LENGTH_HALFHALF = PRINTF_LENGTH_NONE,
  PRINTF_LENGTH_LONG,
  PRINTF_LENGTH_LONGLONG,
  PRINTF_LENGTH_INTMAX,
  PRINTF_LENGTH_SIZE,
  PRINTF_LENGTH_PTRDIFF,
  PRINTF_LENGTH_MAX
} printf_length;

/* float-point specifiers not supported */
typedef enum
{
  PRINTF_SPEC_SIGNED_DECIMAL,
  PRINTF_SPEC_UNSIGNED_DECIMAL,
  PRINTF_SPEC_UNSIGNED_BINARY,
  PRINTF_SPEC_UNSIGNED_OCTAL,
  PRINTF_SPEC_UNSIGNED_HEX,
  PRINTF_SPEC_CHAR,
  PRINTF_SPEC_STRING,
  PRINTF_SPEC_POINTER,
  PRINTF_SPEC_MAX
} printf_spec;

typedef enum
{
  PRINTF_TYPE_UNDEFINED = 0,
  PRINTF_TYPE_UINT,
  PRINTF_TYPE_INT,
  PRINTF_TYPE_ULONG,
  PRINTF_TYPE_LONG,
  PRINTF_TYPE_ULONGLONG,
  PRINTF_TYPE_LONGLONG,
  PRINTF_TYPE_UINTMAX,
  PRINTF_TYPE_INTMAX,
  PRINTF_TYPE_SIZE,
  PRINTF_TYPE_PTRDIFF,
  PRINTF_TYPE_POINTER
} printf_type;

static unsigned char printf_spec_table[PRINTF_LENGTH_MAX][PRINTF_SPEC_MAX]
    = { [PRINTF_LENGTH_NONE]
        = { [PRINTF_SPEC_SIGNED_DECIMAL] = PRINTF_TYPE_INT,
            [PRINTF_SPEC_UNSIGNED_DECIMAL] = PRINTF_TYPE_UINT,
            [PRINTF_SPEC_UNSIGNED_BINARY] = PRINTF_TYPE_UINT,
            [PRINTF_SPEC_UNSIGNED_OCTAL] = PRINTF_TYPE_UINT,
            [PRINTF_SPEC_UNSIGNED_HEX] = PRINTF_TYPE_UINT,
            [PRINTF_SPEC_CHAR] = PRINTF_TYPE_INT,
            [PRINTF_SPEC_STRING] = PRINTF_TYPE_POINTER,
            [PRINTF_SPEC_POINTER] = PRINTF_TYPE_POINTER },
        [PRINTF_LENGTH_LONG] = {
            [PRINTF_SPEC_SIGNED_DECIMAL] = PRINTF_TYPE_LONG,
            [PRINTF_SPEC_UNSIGNED_DECIMAL] = PRINTF_TYPE_ULONG,
            [PRINTF_SPEC_UNSIGNED_BINARY] = PRINTF_TYPE_ULONG,
            [PRINTF_SPEC_UNSIGNED_OCTAL] = PRINTF_TYPE_ULONG,
            [PRINTF_SPEC_UNSIGNED_HEX] = PRINTF_TYPE_ULONG,
        },
        [PRINTF_LENGTH_LONGLONG] = {
            [PRINTF_SPEC_SIGNED_DECIMAL] = PRINTF_TYPE_LONGLONG,
            [PRINTF_SPEC_UNSIGNED_DECIMAL] = PRINTF_TYPE_ULONGLONG,
            [PRINTF_SPEC_UNSIGNED_BINARY] = PRINTF_TYPE_ULONGLONG,
            [PRINTF_SPEC_UNSIGNED_OCTAL] = PRINTF_TYPE_ULONGLONG,
            [PRINTF_SPEC_UNSIGNED_HEX] = PRINTF_TYPE_ULONGLONG,
        },
        [PRINTF_LENGTH_INTMAX] = {
          [PRINTF_SPEC_SIGNED_DECIMAL] = PRINTF_TYPE_INTMAX,
          [PRINTF_SPEC_UNSIGNED_DECIMAL] = PRINTF_TYPE_UINTMAX,
          [PRINTF_SPEC_UNSIGNED_BINARY] = PRINTF_TYPE_UINTMAX,
          [PRINTF_SPEC_UNSIGNED_OCTAL] = PRINTF_TYPE_UINTMAX,
          [PRINTF_SPEC_UNSIGNED_HEX] = PRINTF_TYPE_UINTMAX,
        },
        [PRINTF_LENGTH_SIZE] = {
          [PRINTF_SPEC_SIGNED_DECIMAL] = PRINTF_TYPE_SIZE,
          [PRINTF_SPEC_UNSIGNED_DECIMAL] = PRINTF_TYPE_SIZE,
          [PRINTF_SPEC_UNSIGNED_BINARY] = PRINTF_TYPE_SIZE,
          [PRINTF_SPEC_UNSIGNED_OCTAL] = PRINTF_TYPE_SIZE,
          [PRINTF_SPEC_UNSIGNED_HEX] = PRINTF_TYPE_SIZE,
        },
        [PRINTF_LENGTH_PTRDIFF] = {
          [PRINTF_SPEC_SIGNED_DECIMAL] = PRINTF_TYPE_PTRDIFF,
          [PRINTF_SPEC_UNSIGNED_DECIMAL] = PRINTF_TYPE_PTRDIFF,
          [PRINTF_SPEC_UNSIGNED_BINARY] = PRINTF_TYPE_PTRDIFF,
          [PRINTF_SPEC_UNSIGNED_OCTAL] = PRINTF_TYPE_PTRDIFF,
          [PRINTF_SPEC_UNSIGNED_HEX] = PRINTF_TYPE_PTRDIFF,
        } };

typedef struct
{
  unsigned char base, is_signed, is_zero, is_neg, is_upper;
  union
  {
    unsigned long long llu;
    long long ll;

    uintmax_t mu;
    intmax_t m;

    size_t s;

    ptrdiff_t pd;

    void *p;
  };
} printf_arg;

typedef struct
{
  printf_flags flags;
  printf_length length;
  printf_spec spec;
  int width, prec;
  printf_arg value;
} printf_args;

static const char digits[] = "0123456789abcdef";

void
root_fcursorsync (int fd)
{
  root_ioctl (fd, ROOT_TERM_IOCTL_SYNC_CURSOR);
}

void
root_fputchar (int fd, char ch)
{
  root_write (fd, &ch, 1);
  root_cursorsync ();
}

void
root_fputchar_u (int fd, char ch)
{
  root_write (fd, &ch, 1);
}

static int
printint (printf_args *pargs, char *buf, root_size_t *cursor, root_size_t size)
{
  char tmp[64];
  int len = 0, nlen = 0, pad = 0;
  unsigned long long n;
  printf_arg *value = &pargs->value;
  if (value->is_neg)
    {
      if (*cursor < size)
        buf[(*cursor)++] = '-';
      value->ll = -(value->ll + 1);
      value->llu++;
      len++;
    }
  n = value->llu;
  if (n == 0)
    {
      int prec = pargs->prec > -1 ? pargs->prec : 1;
      nlen = MIN (1, prec);
      tmp[0] = '0';
      goto writeint;
    }
  while (n != 0)
    {
      unsigned long long mod = n % value->base;
      char ch = digits[mod];
      if (value->is_upper && mod > 9)
        ch -= 0x20;
      tmp[nlen++] = ch;
      n /= value->base;
    }
writeint:
  len += nlen;
  if (pargs->prec > -1)
    {
      if (pargs->prec > nlen)
        pad = pargs->prec - nlen;
    }
  else if (pargs->flags & PRINTF_FLAG_LPAD_ZERO && pargs->width > len)
    pad = pargs->width - len;
  for (int i = 0; i < pad && *cursor < size; i++)
    buf[(*cursor)++] = '0';
  len += pad;
  for (int i = 0; i < nlen && *cursor < size; i++)
    buf[(*cursor)++] = tmp[nlen - i - 1];
  return len;
}

int
root_vsnprintf (char *buf, root_size_t size, const char *fmt, va_list args)
{
  printf_args pargs;
  root_size_t cursor = 0, precursor;
  int len = 0, slen;
  char ch;
  printf_type type;
eval:
  ch = *fmt++;
  if (ch == '\0')
    {
      if (size)
        {
          if (cursor >= size)
            buf[size - 1] = '\0';
          else
            buf[cursor] = '\0';
        }
      return len;
    }
  if (ch != '%')
    {
      len++;
      if (cursor < size)
        buf[cursor++] = ch;
      goto eval;
    }
  ch = *fmt++;
  if (ch == '%')
    {
      len++;
      if (cursor < size)
        buf[cursor++] = '%';
      goto eval;
    }
  pargs = (printf_args){ .flags = 0,
                         .length = PRINTF_LENGTH_NONE,
                         .width = -1,
                         .prec = -1,
                         .value = { .base = 10 } };
read_flags:
// TODO: implement warnings for dup flags
#define FLAG_CASE(CH, FLAG)                                                   \
  case CH:                                                                    \
    pargs.flags |= FLAG;                                                      \
    break
  switch (ch)
    {
      FLAG_CASE ('-', PRINTF_FLAG_LEFT_JUSTIFY);
      FLAG_CASE ('+', PRINTF_FLAG_FORCE_SIGN);
      FLAG_CASE (' ', PRINTF_FLAG_BLANK_SPACE);
      FLAG_CASE ('#', PRINTF_FLAG_PREFIX);
      FLAG_CASE ('0', PRINTF_FLAG_LPAD_ZERO);
    default:
      goto end_flags;
    }
  ch = *fmt++;
  goto read_flags;
#undef FLAG_CASE
end_flags:
  if (ch == '*')
    {
      pargs.width = va_arg (args, int);
      ch = *fmt++;
      goto end_width;
    }
  else if (ch < '1' || ch > '9')
    goto end_width;
  pargs.width = 0;
  while (ch >= '0' && ch <= '9')
    {
      pargs.width = pargs.width * 10 + (ch - '0');
      ch = *fmt++;
    }
end_width:
  if (ch == '.')
    {
      pargs.prec = 0;
      ch = *fmt++;
      if (ch == '*')
        {
          pargs.prec = va_arg (args, int);
          ch = *fmt++;
          goto end_prec;
        }
      while (ch >= '0' && ch <= '9')
        {
          pargs.prec = pargs.prec * 10 + (ch - '0');
          ch = *fmt++;
        }
    }
end_prec:
  /* parse length */
#define LENGTH_CASE(CH, LENGTH)                                               \
  case CH:                                                                    \
    ch = *fmt++;                                                              \
    pargs.length = LENGTH;                                                    \
    break
  switch (ch)
    {
    case 'h':
      ch = *fmt++;
      if (ch == 'h')
        {
          pargs.length = PRINTF_LENGTH_HALFHALF;
          ch = *fmt++;
        }
      else
        pargs.length = PRINTF_LENGTH_HALF;
      break;
    case 'l':
      ch = *fmt++;
      if (ch == 'l')
        {
          pargs.length = PRINTF_LENGTH_LONGLONG;
          ch = *fmt++;
        }
      else
        pargs.length = PRINTF_LENGTH_LONG;
      break;
      LENGTH_CASE ('j', PRINTF_LENGTH_INTMAX);
      LENGTH_CASE ('z', PRINTF_LENGTH_SIZE);
      LENGTH_CASE ('t', PRINTF_LENGTH_PTRDIFF);
    }
#undef LENGTH_CASE
    /* parse spec */
#define SPEC_CASE(CH, SPEC)                                                   \
  case CH:                                                                    \
    pargs.spec = SPEC;                                                        \
    break
  switch (ch)
    {
      SPEC_CASE ('d', PRINTF_SPEC_SIGNED_DECIMAL);
      SPEC_CASE ('i', PRINTF_SPEC_SIGNED_DECIMAL);
      SPEC_CASE ('u', PRINTF_SPEC_UNSIGNED_DECIMAL);
    case 'B':
    case 'b':
      pargs.spec = PRINTF_SPEC_UNSIGNED_BINARY;
      pargs.value.base = 2;
      break;
    case 'o':
      pargs.spec = PRINTF_SPEC_UNSIGNED_OCTAL;
      pargs.value.base = 8;
      break;
    case 'X':
      pargs.value.is_upper = 1;
      // fallthrough
    case 'x':
      pargs.spec = PRINTF_SPEC_UNSIGNED_HEX;
      pargs.value.base = 16;
      break;
      SPEC_CASE ('c', PRINTF_SPEC_CHAR);
      SPEC_CASE ('s', PRINTF_SPEC_STRING);
      SPEC_CASE ('p', PRINTF_SPEC_POINTER);
    default:
      /* TODO: emit warning for unknown spec */
      return -1;
    }
#undef SPEC_CASE
  if (pargs.length >= PRINTF_LENGTH_MAX)
    /* TODO: emit warning for invalid length */
    return -1;
  if (pargs.spec >= PRINTF_SPEC_MAX)
    /* TODO: emit warning for invalid spec */
    return -1;
  type = printf_spec_table[pargs.length][pargs.spec];
#define TYPE_CASE(TYPE, MEMB, ARG_TYPE, SIGNED)                               \
  case TYPE:                                                                  \
    pargs.value.MEMB = va_arg (args, ARG_TYPE);                               \
    pargs.value.is_signed = SIGNED;                                           \
    pargs.value.is_zero = pargs.value.MEMB == 0;                              \
    pargs.value.is_neg = pargs.value.MEMB < 0;                                \
    break
  switch (type)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
      TYPE_CASE (PRINTF_TYPE_UINT, llu, unsigned int, 0);
      TYPE_CASE (PRINTF_TYPE_INT, ll, int, 1);
      TYPE_CASE (PRINTF_TYPE_ULONG, llu, unsigned long, 0);
      TYPE_CASE (PRINTF_TYPE_LONG, ll, long, 1);
      TYPE_CASE (PRINTF_TYPE_ULONGLONG, llu, unsigned long long, 0);
      TYPE_CASE (PRINTF_TYPE_LONGLONG, ll, long long, 1);
      TYPE_CASE (PRINTF_TYPE_UINTMAX, mu, uintmax_t, 0);
      TYPE_CASE (PRINTF_TYPE_INTMAX, m, intmax_t, 1);
      TYPE_CASE (PRINTF_TYPE_SIZE, s, size_t, 0);
      TYPE_CASE (PRINTF_TYPE_PTRDIFF, pd, ptrdiff_t, 1);
#pragma GCC diagnostic pop
    case PRINTF_TYPE_POINTER:
      pargs.value.p = va_arg (args, void *);
      pargs.value.is_zero = pargs.value.p == 0;
      break;
    case PRINTF_TYPE_UNDEFINED:
      /* TODO: emit warning for undefined length/spec pair */
      return -1;
    }
#undef TYPE_CASE
  precursor = cursor;
  slen = 0;
  switch (pargs.spec)
    {
    case PRINTF_SPEC_SIGNED_DECIMAL:
    intflags:
      if (pargs.flags & PRINTF_FLAG_FORCE_SIGN
          || pargs.flags & PRINTF_FLAG_BLANK_SPACE)
        {
          if (cursor < size)
            buf[cursor++] = pargs.flags & PRINTF_FLAG_FORCE_SIGN ? '+' : ' ';
          slen++;
        }
    writeint:
      slen += printint (&pargs, buf, &cursor, size);
      len += slen;
      break;
    case PRINTF_SPEC_UNSIGNED_DECIMAL:
      goto intflags;
    case PRINTF_SPEC_UNSIGNED_BINARY:
      if (!pargs.value.is_zero && pargs.flags & PRINTF_FLAG_PREFIX)
        {
          if (cursor < size)
            buf[cursor++] = '0';
          if (cursor < size)
            buf[cursor++] = ch;
          slen += 2;
        }
      goto writeint;
    case PRINTF_SPEC_UNSIGNED_OCTAL:
      if (!pargs.value.is_zero && pargs.flags & PRINTF_FLAG_PREFIX)
        {
          if (cursor < size)
            buf[cursor++] = '0';
          slen++;
        }
      goto writeint;
    case PRINTF_SPEC_UNSIGNED_HEX:
      if (!pargs.value.is_zero && pargs.flags & PRINTF_FLAG_PREFIX)
        {
          if (cursor < size)
            buf[cursor++] = '0';
          if (cursor < size)
            buf[cursor++] = ch;
          slen += 2;
        }
      goto writeint;
    case PRINTF_SPEC_CHAR:
      {
        char c = pargs.value.ll;
        if (cursor < size)
          buf[cursor++] = c;
        slen = 1;
        len++;
        break;
      }
    case PRINTF_SPEC_STRING:
      {
        const char *str = pargs.value.p;
        if (pargs.prec > -1)
          {
            for (int i = 0; str[i] && i < pargs.prec && cursor < size; i++)
              buf[cursor++] = str[i];
            slen = pargs.prec;
          }
        else
          {
            int i;
            for (i = 0; str[i]; i++)
              if (cursor < size)
                buf[cursor++] = str[i];
            slen = i;
          }
        len += slen;
        break;
      }
    case PRINTF_SPEC_POINTER:
      {
        void *p = pargs.value.p;
        pargs.value.llu = (unsigned long long) (uintptr_t) p;
        pargs.value.base = 16;
        if (cursor < size)
          buf[cursor++] = '0';
        if (cursor < size)
          buf[cursor++] = 'x';
        slen += 2;
        goto writeint;
      }
    /* make compiler happy */
    case PRINTF_SPEC_MAX:
      break;
    }
  if (pargs.width > slen)
    {
      root_size_t pad = pargs.width - slen;
      if (pargs.flags & PRINTF_FLAG_LEFT_JUSTIFY)
        for (root_size_t i = 0; i < pad && cursor < size; i++)
          buf[cursor++] = ' ';
      else
        {
          root_size_t start = precursor + pad;
          if (start < size)
            root_memmove (buf + start, buf + precursor,
                          MIN ((root_size_t) slen, size - start));
          for (root_size_t i = precursor; i < pad && i < size; i++)
            buf[i] = ' ';
          cursor = MIN (start + slen, size);
        }
      len += pad;
    }
  goto eval;
}
