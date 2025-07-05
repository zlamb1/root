#ifndef ROOT_FD_H
#define ROOT_FD_H

#include "types.h"

typedef struct root_fd_t
{
  root_ssize_t (*read) (struct root_fd_t *fd, char *buf, root_size_t sz);
  root_ssize_t (*write) (struct root_fd_t *fd, const char *buf,
                         root_size_t sz);
} root_fd_t;

static inline root_size_t
fread (root_fd_t *fd, char *buf, root_size_t sz)
{
  return fd->read (fd, buf, sz);
}

static inline root_size_t
fwrite (root_fd_t *fd, const char *buf, root_size_t sz)
{
  return fd->write (fd, buf, sz);
}

#endif
