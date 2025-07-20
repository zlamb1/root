#include "kern/errno.h"
#ifndef ROOT_DISK_H
#define ROOT_DISK_H 1

#include "kern/types.h"

typedef struct root_disk_t
{
  root_uint32_t nsectors;
  root_ssize_t (*read) (struct root_disk_t *disk, char *buf,
                        root_size_t nsectors);
  root_ssize_t (*write) (struct root_disk_t *disk, const char *buf,
                         root_size_t nsectors);
  struct root_disk_t *next;
} root_disk_t;

void root_disk_register (root_disk_t *disk);
root_err_t root_disk_unregister (root_disk_t *disk);
root_disk_t *root_disk_iterate (root_disk_t *disk);

static root_ssize_t
root_disk_read (root_disk_t *disk, char *buf, root_size_t nsectors)
{
  root_ssize_t read;
  if (disk == NULL || disk->read == NULL || buf == NULL || !nsectors)
    {
      root_seterrno (ROOT_EINVAL);
      return -1;
    }
  read = disk->read (disk, buf, nsectors);
  return read;
}

static root_ssize_t
root_disk_write (root_disk_t *disk, const char *buf, root_size_t nsectors)
{
  root_ssize_t write;
  if (disk == NULL || disk->write == NULL || buf == NULL || !nsectors)
    {
      root_seterrno (ROOT_EINVAL);
      return -1;
    }
  write = disk->write (disk, buf, nsectors);
  return write;
}

#endif
