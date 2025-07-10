#include "cmd/disk.h"
#include "cmd/common.h"
#include "console/print.h"
#include "disk/disk.h"
#include "string.h"

void
root_cmd_disks (int argc, char **argv)
{
  root_disk_t *disk = root_disk_iterate (NULL);
  (void) argc;
  (void) argv;
  while (disk != NULL)
    {
      root_partition_t *part = disk->parts;
      root_printf ("%s\n", disk->name);
      for (int i = 0; part != NULL; i++, part = part->next)
        root_printf ("%s,p%i\n", disk->name, i);
      disk = root_disk_iterate (disk);
    }
}

void
root_cmd_hexdump (int argc, char **argv)
{
  root_disk_t *disk;
  root_size_t offset = 0, length = 0;
  char buf[512];
  for (int i = 1; i < argc; i++)
    {
      if (argv[i][0] != '-')
        {
          disk = root_disk_iterate (NULL);
          while (disk != NULL)
            {
              if (root_strcmp (disk->name, argv[i]) == 0)
                break;
              disk = root_disk_iterate (disk);
            }
          if (disk == NULL)
            {
              root_printf ("err: device not found\n");
              return;
            }
          if (disk->read == NULL)
            {
              root_printf ("err: device does not support read\n");
              return;
            }
          if (offset / 512 >= disk->nsectors)
            {
              root_printf ("err: offset exceeds device capacity\n");
              return;
            }
          if (!length)
            length = disk->nsectors * 512;
          while (length)
            {
              root_size_t read = length > 512 ? 512 : length;
              disk->read (disk, buf, read);
              length -= read;
            }
        }
      else if (root_strcmp (argv[i], "--o") == 0
               || root_strcmp (argv[i], "--offset") == 0)
        {
          int n;
          if (++i >= argc)
            {
              root_printf ("err: expected offset\n");
              return;
            }
          if (root_atoi (argv[i], &n) || n <= 0)
            {
              root_printf ("err: invalid offset '%s'\n", argv[i]);
              return;
            }
          offset = n;
        }
    }
}
