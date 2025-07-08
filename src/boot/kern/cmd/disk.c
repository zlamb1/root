#include "cmd/disk.h"
#include "console/print.h"
#include "disk/disk.h"

void
root_cmd_disks (int argc, char **argv)
{
  root_disk_t *disk = root_disk_iterate (NULL);
  (void) argc;
  (void) argv;
  while (disk != NULL)
    {
      root_printf ("%s\n", disk->name);
      disk = root_disk_iterate (disk);
    }
}
