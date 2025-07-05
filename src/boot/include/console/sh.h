#ifndef ROOT_SHELL_H
#define ROOT_SHELL_H 1

#include "types.h"

typedef void (*cmd_func) (int argc, char **argv);

typedef struct root_command_t
{
  root_size_t name_len;
  const char *name;
  cmd_func func;
} root_command_t;

typedef struct root_shell_t
{
  root_u16 sz, cursor, cap;
  char *buf;
  root_size_t num_cmds, cmds_cap;
  root_command_t *cmds;
} root_shell_t;

root_err_t root_initshell (root_shell_t *sh);

root_err_t root_register_cmd (root_shell_t *sh, const char *name,
                              cmd_func func);

void root_handle_cmd (root_shell_t *sh);

void root_shell_process (root_shell_t *sh);

#endif
