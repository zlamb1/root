#include "kern/cmd.h"
#include "kern/machine.h"
#include "kern/mod.h"

static void
reboot_command (root_cmd_args_t *args)
{
  (void) args;
  root_reboot ();
}

ROOT_MOD_INIT (reboot)
{
  root_register_cmd ("reboot", "reboots machine", reboot_command);
}

ROOT_MOD_FINI (reboot) { root_unregister_cmd ("reboot"); }
