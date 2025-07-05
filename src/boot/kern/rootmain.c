#include <stddef.h>

#include "cmd/common.h"
#include "console/print.h"
#include "console/sh.h"
#include "console/welcome.h"
#include "init.h"
#include "task.h"
#include "types.h"

extern int random (void);

__attribute__ ((noreturn)) void root_main (void);

root_shell_t root_sh;

void
root_main (void)
{
  root_machine_init ();
  root_printwelcome ();
  if (root_initshell (&root_sh) != ROOT_SUCCESS)
    {
      root_printf ("failed to init shell\n");
      for (;;)
        ;
    }
  root_register_common_commands (&root_sh);
  for (;;)
    {
      root_schedule_tasks ();
      root_shell_process (&root_sh);
    }
}
