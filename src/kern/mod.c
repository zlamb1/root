#include "kern/mod.h"
#include "kern/errno.h"
#include "kern/print.h"
#include "kern/types.h"

extern root_initmod_t KERN_INITMODS_START;
extern root_initmod_t KERN_INITMODS_END;

void
root_preload_initmod (root_initmod_t *initmod)
{
  if (initmod->init != NULL)
    initmod->init ();
  else
    root_seterrno (ROOT_EINVAL);
}

void
root_load_initmods (void)
{
  root_initmod_t *mod = (root_initmod_t *) &KERN_INITMODS_START,
                 *end = (root_initmod_t *) &KERN_INITMODS_END;
  root_size_t size = (root_uintptr_t) end - (root_uintptr_t) mod;
  if (size % sizeof (root_initmod_t) != 0)
    {
      root_error ("invalid kern_initmods section", size,
                  sizeof (root_initmod_t));
      return;
    }
  // TODO: resolve deps
  while (mod != end)
    {
      if (mod->loaded)
        {
          mod++;
          continue;
        }
      root_seterrno (ROOT_SUCCESS);
      mod->init ();
      if (root_errno == ROOT_SUCCESS)
        root_ok ("module init: %s", mod->name);
      else
        root_warn ("failed to init module: %s", mod->name);
      mod++;
    }
}
