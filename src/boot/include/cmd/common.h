#ifndef ROOT_CMD_COMMON_H
#define ROOT_CMD_COMMON_H 1

#include "console/sh.h"

typedef struct root_args_t
{
  root_size_t bufc, buf_cap, args_cap;
  char *buf;
  int argc;
  char **argv;
} root_args_t;

int root_atoi (const char *str, int *out);

root_err_t root_parse_args (char *buf, root_size_t sz, root_args_t *args);
root_err_t root_free_args (root_args_t *args);

void root_register_common_commands (root_shell_t *sh);

void root_cmd_clear (int argc, char **argv);
void root_cmd_fg (int argc, char **argv);
void root_cmd_bg (int argc, char **argv);
void root_cmd_args (int argc, char **argv);

#endif
