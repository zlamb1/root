#ifndef ROOT_BDEV_H
#define ROOT_BDEV_H 1

#include "types.h"

typedef struct root_dev_t
{
  char *name;
  root_ssize_t (*read) (struct root_dev_t *dev, char *buf, root_size_t offset,
                        root_size_t size);
  root_ssize_t (*write) (struct root_dev_t *dev, const char *buf,
                         root_size_t offset, root_size_t size);
  struct root_dev_t *next;
} root_dev_t;

root_dev_t *root_dev_iterate (root_dev_t *dev);
void root_dev_register (root_dev_t *dev);
void root_dev_unregister (root_dev_t *dev);

static root_ssize_t
root_dev_read (root_dev_t *dev, char *buf, root_size_t offset,
               root_size_t size)
{
  if (dev == NULL || dev->read == NULL || buf == NULL)
    return ROOT_EINVAL;
  return dev->read (dev, buf, offset, size);
}

static root_ssize_t
root_dev_write (root_dev_t *dev, const char *buf, root_size_t offset,
                root_size_t size)
{
  if (dev == NULL || dev->write == NULL || buf == NULL)
    return ROOT_EINVAL;
  return dev->write (dev, buf, offset, size);
}

#endif
