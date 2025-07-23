#include <stddef.h>

#include "common/page.h"
#include "kern/cmd.h"
#include "kern/errno.h"
#include "kern/input.h"
#include "kern/kc.h"
#include "kern/malloc.h"
#include "kern/mod.h"
#include "kern/print.h"
#include "kern/string.h"
#include "kern/types.h"
#include "mods/sh.h"

typedef struct
{
  int cursor, len, cap;
  char *buf;
} root_shell_t;

static root_shell_t *sh = NULL;

static const char *prompt = "\033[0mroot> ";

static root_cmd_args_t *
sh_parse_args (char *buf, root_size_t len, root_size_t cap)
{
  root_cmd_args_t *args = root_zalloc (sizeof (root_cmd_args_t));
  root_size_t i;
  int parsing = 0;
  if (args == NULL)
    return NULL;
  /* TODO: keep this allocated */
  args->argv = root_zalloc (((cap + 1) >> 1) * sizeof (char *));
  if (args->argv == NULL)
    {
      root_free (args);
      return NULL;
    }
  for (i = 0; i < len; i++)
    {
      if (buf[i] == ' ')
        {
          if (parsing)
            {
              parsing = 0;
              buf[i] = '\0';
            }
          continue;
        }
      if (!parsing)
        {
          parsing = 1;
          args->argv[args->argc++] = buf + i;
        }
    }
  if (parsing && i < cap)
    buf[i] = '\0';
  return args;
}

static void
root_shell_handle_input (root_keycode_t kc, char ascii)
{
  switch (kc)
    {
    case ROOT_KEY_LEFT:
      if (sh->cursor)
        {
          sh->cursor--;
          root_printf ("\b");
        }
      break;
    case ROOT_KEY_RIGHT:
      if (sh->cursor < sh->len)
        root_printf ("%c", sh->buf[sh->cursor++]);
      break;
    case ROOT_KEY_BACKSPACE:
      if (sh->cursor)
        {
          root_memmove (sh->buf + sh->cursor - 1, sh->buf + sh->cursor,
                        sh->len - sh->cursor);
          sh->cursor--;
          sh->len--;
          root_printf ("\b\033[s \b%.*s \033[u", sh->len - sh->cursor,
                       sh->buf + sh->cursor);
        }
      break;
    case ROOT_KEY_ENTER:
      {
        root_cmd_args_t *args = sh_parse_args (sh->buf, sh->len, sh->cap);
        root_cmd_t *cmd;
        sh->len = 0;
        sh->cursor = 0;
        root_printf ("\n");
        if (args == NULL)
          root_error ("out of memory");
        if (args->argc)
          {
            const char *cmd_name = args->argv[0];
            cmd = root_get_cmd (cmd_name);
            if (cmd == NULL)
              root_printf ("%s: \033[91mcommand not found\n", cmd_name);
            else
              cmd->cmd (args);
          }
        root_free (args->argv);
        root_free (args);
        root_printf ("%s", prompt);
        break;
      }
    default:
      if (ascii && sh->len + 1 < sh->cap)
        {
          root_memmove (sh->buf + sh->cursor + 1, sh->buf + sh->cursor,
                        sh->len - sh->cursor);
          sh->buf[sh->cursor++] = ascii;
          sh->len++;
          root_printf ("%c\033[s%.*s\033[u", ascii, sh->len - sh->cursor,
                       sh->buf + sh->cursor);
        }
      break;
    }
}

void
root_shell_task (void)
{
  root_input_event_t evt;
  if (sh == NULL)
    {
      root_seterrno (ROOT_EINVAL);
      return;
    }
  root_printf ("%s", prompt);
  for (;;)
    {
      if (root_poll_input (&evt) && evt.state != ROOT_KEY_RELEASE)
        root_shell_handle_input (evt.kc, evt.ascii);
    }
}

ROOT_MOD_INIT (sh)
{
  root_size_t cap = ROOT_PAGE_SIZE;
  sh = root_zalloc (sizeof (root_shell_t));
  if (sh == NULL)
    {
      root_seterrno (ROOT_EALLOC);
      return;
    }
  sh->buf = root_malloc (cap);
  sh->buf[cap - 1] = '\0';
  sh->cap = cap - 1;
  if (sh->buf == NULL)
    {
      root_free (sh);
      root_seterrno (ROOT_EALLOC);
      return;
    }
}

ROOT_MOD_FINI (sh)
{
  if (sh != NULL)
    {
      root_free (sh->buf);
      root_free (sh);
    }
}
