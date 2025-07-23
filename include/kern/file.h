#ifndef ROOT_FILE_H
#define ROOT_FILE_H 1

#include <stdarg.h>

#include "errno.h"
#include "types.h"

typedef struct root_file_t
{
  root_ssize_t (*read) (struct root_file_t *file, char *buf, root_size_t size);
  root_ssize_t (*write) (struct root_file_t *file, const char *buf,
                         root_size_t size);
  int (*ioctl) (struct root_file_t *file, int op, va_list args);
} root_file_t;

static inline root_ssize_t
root_file_read (root_file_t *file, char *buf, root_size_t size)
{
  if (file == NULL || file->read == NULL)
    return ROOT_EINVAL;
  return file->read (file, buf, size);
}

static inline root_ssize_t
root_file_write (root_file_t *file, const char *buf, root_size_t size)
{
  if (file == NULL || file->write == NULL)
    return ROOT_EINVAL;
  return file->write (file, buf, size);
}

static int
root_file_ioctlv (root_file_t *file, int op, va_list args)
{
  if (file == NULL || file->ioctl == NULL)
    {
      root_seterrno (ROOT_EINVAL);
      return -1;
    }
  return file->ioctl (file, op, args);
}

static int
root_file_ioctl (root_file_t *file, int op, ...)
{
  int result;
  va_list args;
  va_start (args, op);
  result = root_file_ioctlv (file, op, args);
  va_end (args);
  return result;
}

#endif
