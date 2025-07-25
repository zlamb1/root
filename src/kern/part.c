#include "kern/part.h"
#include "kern/errno.h"

static root_partmap_t *partmaps = NULL;

root_partmap_t *
root_partmap_iterate (root_partmap_t *partmap)
{
  return partmap == NULL ? partmaps : partmap->next;
}

void
root_register_partmap (root_partmap_t *partmap)
{
  if (partmap == NULL)
    {
      root_seterrno (ROOT_EINVAL);
      return;
    }
  partmap->next = partmaps;
  partmaps = partmap;
}

void
root_unregister_partmap (root_partmap_t *partmap)
{
  root_partmap_t *pr = NULL, *pm = partmaps;
  while (pm != NULL)
    {
      if (pm == partmap)
        {
          if (pr == NULL)
            partmaps = pm->next;
          else
            pr->next = pm->next;
          partmap->next = NULL;
          return;
        }
      pr = pm;
      pm = pm->next;
    }
  root_seterrno (ROOT_ENOENT);
}
