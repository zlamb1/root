#ifndef ROOT_PART_H
#define ROOT_PART_H 1

#include "kern/dev.h"

struct root_disk_t;

typedef enum
{
  ROOT_PARTITION_TYPE_PRIMARY,
  ROOT_PARTITION_TYPE_EXTENDED,
  ROOT_PARTITION_TYPE_LOGICAL
} root_partition_type_t;

typedef struct root_partition_t
{
  root_dev_t dev;
  root_partition_type_t type;
  unsigned int sec, nsec;
  struct root_disk_t *disk;
  struct root_partition_t *next;
} root_partition_t;

typedef struct root_partmap_t
{
  int (*probe) (root_dev_t *dev);
  struct root_partmap_t *next;
} root_partmap_t;

root_partition_t *root_init_partition (struct root_disk_t *disk);

root_partmap_t *root_partmap_iterate (root_partmap_t *partmap);
void root_register_partmap (root_partmap_t *partmap);
void root_unregister_partmap (root_partmap_t *partmap);

#endif
