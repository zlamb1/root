#ifndef ROOT_DISK_H
#define ROOT_DISK_H 1

#include "dev.h"
#include "types.h"

#define MIN(X, Y) ((X) > (Y) ? (Y) : (X))

#define ROOT_SECTOR_SIZE 512

struct root_partition_t;

typedef struct root_disk_t
{
  root_dev_t dev;
  root_uint32_t sector_size, total_sectors;
  root_ssize_t (*disk_read) (struct root_disk_t *disk, char *buf,
                             root_size_t sec, root_size_t nsec);
  root_ssize_t (*disk_write) (struct root_disk_t *disk, const char *buf,
                              root_size_t sec, root_size_t nsec);
  struct root_partition_t *partitions;
  struct root_disk_t *next;
} root_disk_t;

void root_init_disk_dev (root_disk_t *disk);
void root_free_disk_dev (root_disk_t *disk);

void root_disk_register (root_disk_t *disk);
void root_disk_unregister (root_disk_t *disk);
root_disk_t *root_disk_iterate (root_disk_t *disk);

root_ssize_t root_disk_read (root_disk_t *disk, char *buf, root_size_t offset,
                             root_size_t size);

#endif
