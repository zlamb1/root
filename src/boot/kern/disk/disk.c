#include "disk/disk.h"
#include "console/print.h"
#include "memory/malloc.h"

root_disk_t *root_disk = NULL;

static unsigned char hdcnt = 0;

root_disk_t *
root_disk_iterate (root_disk_t *disk)
{
  if (disk == NULL)
    return root_disk;
  else
    return disk->next;
}

root_err_t
root_disk_register (root_disk_t *disk)
{
  char *name;
  int dn = hdcnt, i = 0;
  if (disk == NULL)
    return ROOT_EARG;
  name = root_malloc (8);
  if (name == NULL)
    return ROOT_EALLOC;
  root_sprintf (name, "hd%u", hdcnt++);
  disk->name = name;
  disk->next = root_disk;
  root_disk = disk;
  return ROOT_SUCCESS;
}
