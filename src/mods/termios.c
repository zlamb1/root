#include "kern/cmd.h"
#include "kern/mod.h"

static void
term_command (root_cmd_args_t *args)
{
  (void) args;
}

static void
fg_command (root_cmd_args_t *args)
{
  (void) args;
}

ROOT_MOD_INIT (termios) {}

ROOT_MOD_FINI (termios) {}
