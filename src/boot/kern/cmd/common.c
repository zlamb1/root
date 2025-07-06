#include "cmd/common.h"
#include "console/print.h"
#include "console/sh.h"
#include "console/welcome.h"
#include "machine.h"
#include "memory/alloc.h"
#include "memory/page.h"
#include "types.h"

static int
root_atoi_h (const char *str, int *out)
{
  int n = 0;
  char ch = *str;
  while (ch != '\0')
    {
      switch (ch)
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          n = n * 16 + (ch - '0');
          break;
        case 'a':
        case 'A':
          n = n * 16 + 10;
          break;
        case 'b':
        case 'B':
          n = n * 16 + 11;
          break;
        case 'c':
        case 'C':
          n = n * 16 + 12;
          break;
        case 'd':
        case 'D':
          n = n * 16 + 13;
          break;
        case 'e':
        case 'E':
          n = n * 16 + 14;
          break;
        case 'f':
        case 'F':
          n = n * 16 + 15;
          break;
        default:
          return 1;
        }
      ch = *++str;
    }
  *out = n;
  return ROOT_SUCCESS;
}

int
root_atoi (const char *str, int *out)
{
  int n = 0;
  char ch;
  if (str == NULL || out == NULL)
    return ROOT_ERR_ARG;
  ch = *str;
  if (ch == '0')
    {
      ch = *++str;
      if (ch == 'x' || ch == 'X')
        return root_atoi_h (++str, out);
    }
  while (ch != '\0')
    {
      if (ch < '0' || ch > '9')
        return 1;
      n = n * 10 + (ch - '0');
      ch = *++str;
    }
  *out = n;
  return 0;
}

root_err_t
root_parse_args (char *buf, root_size_t sz, root_args_t *out)
{
  // FIXME: use realloc
  root_args_t args = { 0 };
  int read = 0;
  if (buf == NULL || out == NULL)
    return ROOT_ERR_ARG;
  args.buf = root_alloc_pages (1);
  if (args.buf == NULL)
    return ROOT_ERR_ALLOC;
  args.argv = root_alloc_pages (1);
  if (args.argv == NULL)
    {
      root_free_pages (args.buf);
      return ROOT_ERR_ALLOC;
    }
  args.buf_cap = ROOT_PAGE_SIZE;
  args.args_cap = ROOT_PAGE_SIZE;
  while (sz--)
    {
      char ch = *buf++;
      switch (ch)
        {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
          if (read)
            {
              args.buf[args.bufc++] = '\0';
              args.argc++;
              read = 0;
            }
          break;
        default:
          if (!read)
            args.argv[args.argc] = args.buf + args.bufc;
          args.buf[args.bufc++] = ch;
          read++;
          break;
        }
    }
  if (read)
    {
      args.buf[args.bufc++] = '\0';
      args.argc++;
    }
  *out = args;
  return ROOT_SUCCESS;
}

root_err_t
root_free_args (root_args_t *args)
{
  if (args == NULL)
    return ROOT_ERR_ARG;
  args->bufc = 0;
  args->buf_cap = 0;
  args->argc = 0;
  args->args_cap = 0;
  if (args->buf != NULL)
    {
      root_free_pages (args->buf);
      args->buf = NULL;
    }
  if (args->argv != NULL)
    {
      root_free_pages (args->argv);
      args->argv = NULL;
    }
  return ROOT_SUCCESS;
}

void
root_register_common_commands (root_shell_t *sh)
{
  root_register_cmd (sh, "clear", root_cmd_clear);
  root_register_cmd (sh, "fg", root_cmd_fg);
  root_register_cmd (sh, "bg", root_cmd_bg);
  root_register_cmd (sh, "args", root_cmd_args);
  root_register_cmd (sh, "reboot", root_cmd_reboot);
  root_register_cmd (sh, "welcome", root_cmd_welcome);
}

void
root_cmd_clear (int argc, char **argv)
{
  (void) argc;
  (void) argv;
  root_console->clear (root_console, root_console->bg);
}

void
root_cmd_fg (int argc, char **argv)
{
  int fg;
  if (argc < 2)
    {
      root_console->fg = 0x00AA00;
      return;
    }
  if (root_atoi (argv[1], &fg))
    {
      root_printf ("fg: expected numeric foreground argument\n");
      return;
    }
  root_console->fg = fg;
}

void
root_cmd_bg (int argc, char **argv)
{
  int bg;
  if (argc < 2)
    {
      root_console->bg = 0;
      return;
    }
  if (root_atoi (argv[1], &bg))
    {
      root_printf ("bg: expected numeric background argument\n");
      return;
    }
  root_console->bg = bg;
}

void
root_cmd_args (int argc, char **argv)
{
  for (int i = 0; i < argc; i++)
    root_printf ("%s\n", argv[i]);
}

void
root_cmd_reboot (int argc, char **argv)
{
  (void) argc;
  (void) argv;
  root_mach_reboot ();
}
