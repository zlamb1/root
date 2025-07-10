#include "disk/disk.h"
#include "console/print.h"
#include "memory/malloc.h"
#include "types.h"

typedef struct
{
  root_uint8_t status;
  root_uint8_t start_head;
  root_uint8_t start_sector_and_cylinder_hi;
  root_uint8_t start_cylinder_lo;
  root_uint8_t type;
  root_uint8_t end_head;
  root_uint8_t end_sector_and_cylinder_hi;
  root_uint8_t end_cylinder_lo;
  root_uint32_t start_lba;
  root_uint32_t sector_count;
} __attribute__ ((packed)) partition_entry_t;

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

static void
root_disk_read_partitions (root_disk_t *disk)
{
  root_uint8_t mbr[ROOT_SECTOR_SIZE];
  partition_entry_t *part_entry;
  if (disk->read == NULL)
    return;
  if (disk->read (disk, mbr, ROOT_SECTOR_SIZE) != ROOT_SECTOR_SIZE)
    return;
  part_entry = (partition_entry_t *) (mbr + 446);
  for (int i = 0; i < 4; i++, part_entry++)
    {
      if (part_entry->type)
        {
          root_partition_t *part;
          if (part_entry->start_lba >= disk->nsectors)
            // TODO: emit warning
            continue;
          if (part_entry->start_lba + part_entry->sector_count
              > disk->nsectors)
            // TODO: emit warning
            continue;
          part = root_malloc (sizeof (root_partition_t));
          if (part == NULL)
            // TODO: emit warning
            continue;
          part->disk = disk;
          part->status = part_entry->status;
          part->type = part_entry->type;
          part->lba = part_entry->start_lba;
          part->sector_count = part_entry->sector_count;
          part->next = disk->parts;
          disk->parts = part;
        }
    }
}

root_err_t
root_disk_register (root_disk_t *disk)
{
  char *name;
  int dn = hdcnt;
  if (disk == NULL)
    return ROOT_EARG;
  if (disk->name == NULL)
    {
      name = root_malloc (8);
      if (name == NULL)
        return ROOT_EALLOC;
      root_sprintf (name, "hd%u", hdcnt++);
      disk->name = name;
    }
  if (disk->parts == NULL)
    root_disk_read_partitions (disk);
  disk->next = root_disk;
  root_disk = disk;
  return ROOT_SUCCESS;
}

root_err_t
root_disk_free (root_disk_t *disk)
{
  root_err_t err = ROOT_SUCCESS;
  root_partition_t *part;
  if (disk == NULL)
    return ROOT_EARG;
  if (disk->free != NULL)
    err = disk->free (disk);
  if (disk->name != NULL)
    root_free (disk->name);
  part = disk->parts;
  while (part != NULL)
    {
      root_partition_t *next = part->next;
      root_free (part);
      part = next;
    }
  root_free (disk);
  return err;
}
