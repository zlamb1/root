#include "kern/dev.h"
#include "kern/errno.h"

static root_dev_t *devs = NULL;

root_dev_t *
root_dev_iterate (root_dev_t *dev)
{
  return dev == NULL ? devs : dev->next;
}

void
root_dev_register (root_dev_t *dev)
{
  if (dev == NULL)
    {
      root_seterrno (ROOT_EINVAL);
      return;
    }
  dev->next = devs;
  devs = dev;
}

void
root_dev_unregister (root_dev_t *dev)
{
  root_dev_t *p = NULL, *b;
  if (dev == NULL)
    {
      root_seterrno (ROOT_EINVAL);
      return;
    }
  b = devs;
  while (b != NULL)
    {
      if (b == dev)
        {
          if (p == NULL)
            devs = dev->next;
          else
            p->next = dev->next;
          dev->next = NULL;
          return;
        }
      p = b;
      b = b->next;
    }
  root_seterrno (ROOT_ENOENT);
}
