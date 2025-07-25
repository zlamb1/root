#include "kern/disk.h"
#include "kern/dev.h"
#include "kern/errno.h"
#include "kern/malloc.h"
#include "kern/part.h"
#include "kern/print.h"
#include "kern/types.h"

static root_disk_t *disks = NULL;

static root_ssize_t
root_disk_dev_read (root_dev_t *dev, char *buf, root_size_t offset,
                    root_size_t size)
{
  return root_disk_read ((root_disk_t *) dev, buf, offset, size);
}

void
root_init_disk_dev (root_disk_t *disk)
{
  static unsigned n = 0;
  int len;
  if (disk == NULL)
    {
      root_seterrno (ROOT_EINVAL);
      return;
    }
  len = root_snprintf (NULL, 0, "hd%u", n);
  if (len > 2 || (disk->dev.name = root_malloc (len + 1)) == NULL)
    root_snprintf (disk->dev.name, len + 1, "hd%u", n++);
  else
    root_warn ("root_init_disk_dev: failed to create disk device name");
  disk->dev.read = root_disk_dev_read;
  // TODO: support write operations
  disk->dev.write = NULL;
}

void
root_free_disk_dev (root_disk_t *disk)
{
  if (disk == NULL)
    {
      root_seterrno (ROOT_EINVAL);
      return;
    }
  if (disk->dev.name != NULL)
    root_free (disk->dev.name);
}

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
  root_dev_register (&disk->dev);
}

void
root_disk_unregister (root_disk_t *disk)
{
  root_disk_t *prev = NULL, *iter = disks;
  struct root_partition_t *part;
  if (disk == NULL)
    {
      root_seterrno (ROOT_EINVAL);
      return;
    }
  part = disk->partitions;
  while (part != NULL)
    {
      root_partition_t *next = part->next;
      root_dev_unregister (&part->dev);
      root_free (part);
      part = next;
    }
  disk->partitions = NULL;
  while (iter != NULL)
    {
      if (iter == disk)
        {
          if (prev == NULL)
            disks = disk->next;
          else
            prev->next = disk->next;
          disk->next = NULL;
          root_dev_unregister (&disk->dev);
          return;
        }
      prev = iter;
      iter = iter->next;
    }
  root_seterrno (ROOT_ENOENT);
}

root_disk_t *
root_disk_iterate (root_disk_t *disk)
{
  return disk == NULL ? disks : disk->next;
}

root_ssize_t
root_disk_read (root_disk_t *disk, char *buf, root_size_t offset,
                root_size_t size)
{
  root_size_t sector, tmpsize = size, tmp, tmpoffset;
  root_ssize_t tmpread;
  char tmpbuf[ROOT_SECTOR_SIZE];
  if (disk == NULL || disk->disk_read == NULL || buf == NULL)
    {
      root_seterrno (ROOT_EINVAL);
      return -1;
    }
  sector = offset / ROOT_SECTOR_SIZE;
  if (!size || sector >= disk->nsec)
    return 0;
  tmp = sector * ROOT_SECTOR_SIZE;
  if (tmp != offset)
    {
      tmpoffset = offset - tmp;
      tmpread = disk->disk_read (disk, tmpbuf, sector, 1);
      if (tmpread < 0)
        {
          root_seterrno (tmpread);
          return -1;
        }
      else if (tmpread != 1)
        {
          root_warn ("root_disk_read: read invalid number of sectors");
          root_seterrno (ROOT_EIO);
          return -1;
        }
      tmpread = MIN (size, ROOT_SECTOR_SIZE - tmpoffset);
      root_memcpy (buf, tmpbuf + tmpoffset, tmpread);
      buf += tmpread;
      size -= tmpread;
      if (!size)
        goto end;
      sector++;
    }
  tmp = size / ROOT_SECTOR_SIZE;
  if (tmp)
    {
      tmp = MIN (tmp, disk->nsec - sector);
      if (!tmp)
        goto end;
      tmpread = disk->disk_read (disk, buf, sector, tmp);
      if (tmpread < 0)
        {
          root_seterrno (tmpread);
          return -1;
        }
      if ((root_size_t) tmpread != tmp)
        {
          root_warn ("root_disk_read: read invalid number of sectors");
          root_seterrno (ROOT_EIO);
          return -1;
        }
      tmpread = tmp * ROOT_SECTOR_SIZE;
      buf += tmpread;
      size -= tmpread;
      sector += tmp;
    }
  if (size && sector < disk->nsec)
    {
      if (size >= ROOT_SECTOR_SIZE)
        {
          root_warn ("root_disk_read: invalid size");
          root_seterrno (ROOT_EIO);
          return -1;
        }
      tmpread = disk->disk_read (disk, tmpbuf, sector, 1);
      if (tmpread < 0)
        {
          root_seterrno (tmpread);
          return -1;
        }
      else if (tmpread != 1)
        {
          root_warn ("root_disk_read: read invalid number of sectors");
          root_seterrno (ROOT_EIO);
          return -1;
        }
      root_memcpy (buf, tmpbuf, size);
      size = 0;
    }
end:
  return tmpsize - size;
}
