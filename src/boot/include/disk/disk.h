#ifndef ROOT_DISK_H
#define ROOT_DISK_H 1

#include "types.h"

typedef struct root_disk_t
{
  const char *name;
  root_size_t nsectors;
  root_ssize_t (*read) (struct root_disk_t *disk, void *buf, root_size_t sz);
  struct root_disk_t *next;
} root_disk_t;

root_disk_t *root_disk_iterate (root_disk_t *disk);

root_err_t root_disk_register (root_disk_t *disk);

#endif
