#ifndef ROOT_CMD_H
#define ROOT_CMD_H 1

#include "kern/types.h"

typedef struct
{
  int argc;
  char **argv;
} root_cmd_args_t;

typedef struct
{
  char *value;
  struct root_cmd_tab_completion_t *next;
} root_cmd_tab_completion_t;

typedef void (*root_cmd) (root_cmd_args_t *args);
typedef root_cmd_tab_completion_t *(*root_cmd_tab_completer) (
    root_cmd_args_t args);

typedef struct root_cmd_t
{
  const char *name;
  const char *desc;
  root_cmd cmd;
  root_cmd_tab_completer tab_completer;
  struct root_cmd_t *next;
} root_cmd_t;

void root_register_cmd (const char *name, const char *desc, root_cmd cmd);
void root_register_cmdext (const char *name, const char *desc, root_cmd cmd,
                           root_cmd_tab_completer tab_completer);
root_err_t root_unregister_cmd (const char *name);

root_cmd_t *root_cmd_iterate (root_cmd_t *cmd);
root_cmd_t *root_get_cmd (const char *name);

#endif
