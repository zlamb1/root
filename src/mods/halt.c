#include "kern/cmd.h"
#include "kern/machine.h"
#include "kern/mod.h"

static void
halt_command (root_cmd_args_t *args)
{
  (void) args;
  root_halt ();
}

ROOT_MOD_INIT (halt)
{
  root_register_cmd ("halt", "halts machine", halt_command);
}

ROOT_MOD_FINI (halt) { root_unregister_cmd ("halt"); }
