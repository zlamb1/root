#ifndef ROOT_DISK_H
#define ROOT_DISK_H 1

#include "types.h"

#define ROOT_SECTOR_SIZE 512

typedef struct root_partition_t
{
  struct root_disk_t *disk;
  root_uint8_t status, type;
  root_size_t lba, sector_count;
  struct root_partition_t *next;
} root_partition_t;

typedef struct root_disk_t
{
  char *name;
  root_size_t nsectors;
  root_partition_t *parts;
  root_ssize_t (*read) (struct root_disk_t *disk, void *buf, root_size_t sz);
  root_err_t (*free) (struct root_disk_t *disk);
  struct root_disk_t *next;
} root_disk_t;

root_disk_t *root_disk_iterate (root_disk_t *disk);

root_err_t root_disk_register (root_disk_t *disk);

root_err_t root_disk_free (root_disk_t *disk);

#endif
