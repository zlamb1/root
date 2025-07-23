#include "kern/cmd.h"
#include "kern/mod.h"
#include "kern/print.h"

static void
root_hello_command (root_cmd_args_t *args)
{
  (void) args;
  root_printf ("Hello, world!\n");
}

ROOT_MOD_INIT (hello)
{
  root_register_cmd ("hello", "prints \"Hello, world!\"", root_hello_command);
}

ROOT_MOD_FINI (hello) {}
