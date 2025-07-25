#include "kern/dev.h"
#include "kern/cmd.h"
#include "kern/mod.h"
#include "kern/print.h"
#include "mods/sh.h"

static void
dev_command (root_cmd_args_t *args)
{
  root_dev_t *dev;
  (void) args;
  dev = root_dev_iterate (NULL);
  if (dev == NULL)
    {
      root_sh_cmd_error ("no block devices found");
      return;
    }
  while (dev != NULL)
    {
      root_printf ("%s\n", dev->name);
      dev = dev->next;
    }
}

ROOT_MOD_INIT (dev)
{
  root_register_cmd ("dev", "lists devices", dev_command);
}

ROOT_MOD_FINI (dev) { root_unregister_cmd ("dev"); }
