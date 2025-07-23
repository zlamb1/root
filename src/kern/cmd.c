#include "kern/cmd.h"
#include "kern/errno.h"
#include "kern/malloc.h"
#include "kern/string.h"
#include "kern/types.h"

static root_cmd_t *cmds = NULL;

void
root_register_cmd (const char *name, const char *desc, root_cmd cmd)
{
  root_cmd_t *c;
  if (name == NULL || cmd == NULL)
    {
      root_seterrno (ROOT_EINVAL);
      return;
    }
  c = root_malloc (sizeof (root_cmd_t));
  if (c == NULL)
    {
      root_seterrno (ROOT_EALLOC);
      return;
    }
  c->name = name;
  c->desc = desc;
  c->cmd = cmd;
  c->next = cmds;
  cmds = c;
}

root_err_t
root_unregister_cmd (const char *name)
{
  root_cmd_t *prev = NULL, *cmd = cmds;
  while (cmd != NULL)
    {
      if (root_strcmp (name, cmd->name) == 0)
        {
          if (prev == NULL)
            cmds = cmd->next;
          else
            prev->next = cmd->next;
          root_free (cmd);
          return ROOT_SUCCESS;
        }
      prev = cmd;
      cmd = cmd->next;
    }
  return ROOT_ENOENT;
}

root_cmd_t *
root_get_cmd (const char *name)
{
  root_cmd_t *cmd = cmds;
  while (cmd != NULL)
    {
      if (root_strcmp (name, cmd->name) == 0)
        return cmd;
      cmd = cmd->next;
    }
  return NULL;
}
