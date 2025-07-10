#include "console/sh.h"
#include "cmd/common.h"
#include "console/console.h"
#include "console/print.h"
#include "memory/alloc.h"
#include "memory/page.h"
#include "string.h"
#include "types.h"

root_err_t
root_initshell (root_shell_t *sh)
{
  if (sh == NULL)
    return ROOT_EARG;
  sh->sz = 0;
  sh->cap = 0;
  sh->buf = root_alloc_page ();
  sh->num_cmds = 0;
  sh->cmds_cap = ROOT_PAGE_SIZE / sizeof (root_command_t);
  sh->cmds = root_alloc_page ();
  if (sh->buf == NULL || sh->cmds == NULL)
    return ROOT_EALLOC;
  sh->cap = ROOT_PAGE_SIZE;
  return ROOT_SUCCESS;
}

root_err_t
root_register_cmd (root_shell_t *sh, const char *name, cmd_func func)
{
  root_command_t *cmd;
  if (sh->num_cmds >= sh->cmds_cap)
    return ROOT_EALLOC;
  cmd = sh->cmds + sh->num_cmds++;
  cmd->name_len = root_strlen (name);
  cmd->name = name;
  cmd->func = func;
  return ROOT_SUCCESS;
}

void
root_handle_cmd (root_shell_t *sh)
{
  root_args_t args;
  root_err_t err = root_parse_args (sh->buf, sh->sz, &args);
  if (err != ROOT_SUCCESS)
    return;
  if (args.argc)
    {
      char *cmd_name = args.argv[0];
      root_size_t cmd_name_len = root_strlen (cmd_name);
      root_command_t *found_cmd = NULL;
      for (root_size_t i = 0; i < sh->num_cmds; i++)
        {
          root_command_t *cmd = sh->cmds + i;
          if (cmd->name_len == cmd_name_len)
            {
              for (root_size_t j = 0; j < cmd->name_len; j++)
                if (cmd_name[j] != cmd->name[j])
                  break;
                else if (j == cmd->name_len - 1)
                  {
                    found_cmd = cmd;
                    goto hndcmd;
                  }
            }
        }
    hndcmd:
      if (found_cmd != NULL)
        found_cmd->func (args.argc, args.argv);
      else
        root_printf ("unknown command: %.*s\n", sh->sz, sh->buf);
    }
  sh->sz = 0;
  sh->cursor = 0;
  root_free_args (&args);
}

void
root_shell_process (root_shell_t *sh)
{
  root_ssize_t read;
  root_printf ("root> ");
  read = console_read (root_console, sh->buf, sh->cap);
  if (read > 0)
    {
      sh->sz = read;
      root_handle_cmd (sh);
    }
}
