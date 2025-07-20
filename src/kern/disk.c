#include "kern/disk.h"
#include "kern/errno.h"
#include "kern/types.h"

static root_disk_t *disks = NULL;

void
root_disk_register (root_disk_t *disk)
{
  if (disk == NULL)
    {
      root_seterrno (ROOT_EINVAL);
      return;
    }
  disk->next = disks;
  disks = disk;
}

root_err_t
root_disk_unregister (root_disk_t *disk)
{
  root_disk_t *prev = NULL, *iter = disks;
  if (disk == NULL)
    return ROOT_EINVAL;
  while (iter != NULL)
    {
      if (iter == disk)
        {
          if (prev == NULL)
            disks = disk->next;
          else
            prev->next = disk->next;
          disk->next = NULL;
          return ROOT_SUCCESS;
        }
      prev = iter;
      iter = iter->next;
    }
  return ROOT_ENOENT;
}

root_disk_t *
root_disk_iterate (root_disk_t *disk)
{
  return disk == NULL ? disks : disk->next;
}
