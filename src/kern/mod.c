#include "kern/mod.h"
#include "kern/errno.h"
#include "kern/print.h"
#include "kern/types.h"

extern char KERN_INITMODS_START;
extern char KERN_INITMODS_END;

void
root_initmods (void)
{
  root_module_t *mod = (root_module_t *) &KERN_INITMODS_START,
                *end = (root_module_t *) &KERN_INITMODS_END;
  while (mod != end)
    {
      root_seterrno (ROOT_SUCCESS);
      mod->init ();
      if (root_errno == ROOT_SUCCESS)
        root_ok ("module init: %s", mod->name);
      else
        root_warn ("failed to init module: %s", mod->name);
      mod++;
    }
}
