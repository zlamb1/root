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
  root_keycode_t last_kc;
  const char *cmd;
} shell_t;

typedef struct tab_completion_t
{
  const char *str;
  struct tab_completion_t *next;
} tab_completion_t;

static shell_t *root_sh = NULL;

static const char *prompt = "\033[0;94mroot> \033[0m";

static root_cmd_args_t *
sh_parse_args (shell_t *sh)
{
  root_cmd_args_t *args = root_zalloc (sizeof (root_cmd_args_t));
  int i;
  int parsing = 0;
  if (args == NULL)
    return NULL;
  /* TODO: keep this allocated */
  args->argv = root_zalloc (((sh->cap + 1) >> 1) * sizeof (char *));
  if (args->argv == NULL)
    {
      root_free (args);
      return NULL;
    }
  for (i = 0; i < sh->len; i++)
    {
      if (sh->buf[i] == ' ')
        {
          if (parsing)
            {
              parsing = 0;
              sh->buf[i] = '\0';
            }
          continue;
        }
      if (!parsing)
        {
          parsing = 1;
          args->argv[args->argc++] = sh->buf + i;
        }
    }
  if (parsing && i < sh->cap)
    sh->buf[i] = '\0';
  return args;
}

static void
sh_free_args (root_cmd_args_t *args)
{
  root_free (args->argv);
  root_free (args);
}

static void
sh_tab_complete (shell_t *sh, root_cmd_args_t *args, tab_completion_t *head)
{
  if (args == NULL || !args->argc || head == NULL)
    return;
  while (sh->cursor < sh->len)
    {
      root_printf ("%c", sh->buf[sh->cursor]);
      sh->cursor++;
    }
  if (head->next == NULL)
    {
      const char *src;
      char *dst;
      int prelen = root_strlen (args->argv[args->argc - 1]),
          reallen = root_strlen (head->str), diff;
      if (prelen < reallen)
        {
          src = head->str + prelen;
          dst = args->argv[args->argc - 1] + prelen;
          diff = reallen - prelen;
        }
      else if (prelen == reallen)
        {
          dst = args->argv[args->argc - 1] + reallen;
          if (dst != sh->buf + sh->len)
            return;
          src = " ";
          diff = 1;
        }
      else
        {
          root_warn ("sh: invalid tab completion");
          return;
        }
      if (sh->len + diff + 1 >= sh->cap)
        return;
      root_snprintf (dst, diff + 1, "%.*s", diff, src);
      sh->cursor += diff;
      sh->len += diff;
      root_printf ("%.*s", diff, src);
      return;
    }
  root_printf ("\n%s", head->str);
  head = head->next;
  while (head != NULL)
    {
      root_printf (" %s", head->str);
      head = head->next;
    }
  root_printf ("\n%s%.*s", prompt, sh->len, sh->buf);
}

static void
sh_tab_complete_cmd (shell_t *sh)
{
  root_cmd_args_t *args = sh_parse_args (sh);
  root_cmd_t *cmd;
  tab_completion_t *head = NULL, *tail = NULL;
  if (args == NULL)
    return;
  else if (args->argc != 1)
    goto finish;
  cmd = root_get_cmds ();
  while (cmd != NULL)
    {
      if (root_strpre (args->argv[0], cmd->name))
        {
          tab_completion_t *t = root_zalloc (sizeof (tab_completion_t));
          if (t == NULL)
            goto finish;
          t->str = cmd->name;
          if (head == NULL)
            {
              head = t;
              tail = t;
            }
          else
            {
              tail->next = t;
              tail = t;
            }
        }
      cmd = cmd->next;
    }
  sh_tab_complete (sh, args, head);
finish:
  while (head != NULL)
    {
      tab_completion_t *next = head->next;
      root_free (head);
      head = next;
    }
  sh_free_args (args);
}

static void
sh_handle_input (shell_t *sh, root_keycode_t kc, char ascii)
{
  switch (kc)
    {
    case ROOT_KEY_TAB:
      if (sh->last_kc == ROOT_KEY_TAB)
        sh_tab_complete_cmd (sh);
      break;
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
        root_cmd_args_t *args = sh_parse_args (sh);
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
              {
                sh->cmd = cmd->name;
                cmd->cmd (args);
                sh->cmd = NULL;
              }
          }
        sh_free_args (args);
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
  sh->last_kc = kc;
}

void
root_sh_cmd_error (const char *fmt, ...)
{
  va_list args;
  if (root_sh->cmd == NULL)
    root_error ("sh: command error invoked with null command");
  root_printf ("\033[93m%s: \033[91m", root_sh->cmd);
  va_start (args, fmt);
  root_vprintf (fmt, args);
  va_end (args);
  root_printf ("\n");
}

void
root_shell_task (void)
{
  root_input_event_t evt;
  if (root_sh == NULL)
    {
      root_seterrno (ROOT_EINVAL);
      return;
    }
  root_printf ("%s", prompt);
  for (;;)
    {
      if (root_poll_input (&evt) && evt.state != ROOT_KEY_RELEASE)
        sh_handle_input (root_sh, evt.kc, evt.ascii);
    }
}

/* BUILTIN COMMANDS */

static void
sh_command_echo (root_cmd_args_t *args)
{
  for (int i = 1; i < args->argc; i++)
    {
      root_printf ("%s", args->argv[i]);
      if (i != args->argc - 1)
        root_printf (" ");
    }
  if (args->argc > 1)
    root_printf ("\n");
}

static void
sh_command_help (root_cmd_args_t *args)
{
  root_cmd_t *cmd;
  (void) args;
  cmd = root_get_cmds ();
  if (cmd == NULL)
    {
      root_sh_cmd_error ("no commands found");
      return;
    }
  while (cmd != NULL)
    {
      root_printf ("\033[93m%s: ", cmd->name);
      if (cmd->desc == NULL)
        root_printf ("\033[37mno description");
      else
        root_printf ("\033[0m%s", cmd->desc);
      root_printf ("\n");
      cmd = cmd->next;
    }
}

ROOT_MOD_INIT (sh)
{
  root_size_t cap = ROOT_PAGE_SIZE;
  root_sh = root_zalloc (sizeof (shell_t));
  if (root_sh == NULL)
    {
      root_seterrno (ROOT_EALLOC);
      return;
    }
  root_sh->buf = root_malloc (cap);
  if (root_sh->buf == NULL)
    {
      root_free (root_sh);
      root_seterrno (ROOT_EALLOC);
      return;
    }
  root_sh->buf[cap - 1] = '\0';
  root_sh->cap = cap - 1;
  root_register_cmd ("echo", "prints arguments", sh_command_echo);
  root_register_cmd ("help", "prints all commands", sh_command_help);
}

ROOT_MOD_FINI (sh)
{
  if (root_sh != NULL)
    {
      root_free (root_sh->buf);
      root_free (root_sh);
    }
}
