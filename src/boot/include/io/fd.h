#ifndef ROOT_FD_H
#define ROOT_FD_H

#include "string.h"
#include "types.h"

typedef struct root_fd_t
{
  root_ssize_t (*read) (struct root_fd_t *fd, char *buf, root_size_t sz);
  root_ssize_t (*write) (struct root_fd_t *fd, const char *buf,
                         root_size_t sz);
} root_fd_t;

typedef struct
{
  root_fd_t fd;
  root_size_t fsize, foffset;
  char *buf;
} root_fdmem_t;

static inline root_ssize_t
fread (root_fd_t *fd, char *buf, root_size_t sz)
{
  return fd->read (fd, buf, sz);
}

static inline root_ssize_t
fwrite (root_fd_t *fd, const char *buf, root_size_t sz)
{
  return fd->write (fd, buf, sz);
}

static root_ssize_t
fmemread (root_fd_t *fd, char *buf, root_size_t sz)
{
  root_size_t fsz;
  root_fdmem_t *memfd = (root_fdmem_t *) fd;
  if (memfd->foffset >= memfd->fsize)
    return 0;
  fsz = memfd->fsize - memfd->foffset;
  if (fsz < sz)
    sz = fsz;
  root_memcpy (buf, memfd->buf + memfd->foffset, sz);
  memfd->foffset += sz;
  return sz;
}

static root_ssize_t
fmemwrite (root_fd_t *fd, const char *buf, root_size_t sz)
{
  root_size_t fsz;
  root_fdmem_t *memfd = (root_fdmem_t *) fd;
  if (memfd->foffset >= memfd->fsize)
    return 0;
  fsz = memfd->fsize - memfd->foffset;
  if (fsz < sz)
    sz = fsz;
  root_memcpy (memfd->buf + memfd->foffset, buf, sz);
  memfd->foffset += sz;
  return sz;
}

static inline root_fdmem_t
fcreate_memfd (void *buf, root_size_t fsize)
{
  return (root_fdmem_t){
    .fd = { .read = fmemread, .write = fmemwrite },
    .fsize = fsize,
    .foffset = 0,
    .buf = (char *) buf,
  };
}

#endif
