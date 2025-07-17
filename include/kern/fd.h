#include "file.h"
#ifndef ROOT_FD_H
#define ROOT_FD_H 1

#include <stdarg.h>

#include "task.h"
#include "types.h"

static root_ssize_t
root_read (int fd, char *buf, root_size_t size)
{
  if (root_running_task == NULL || (root_size_t) fd >= root_running_task->nfds
      || root_running_task->fds[fd] == NULL)
    return ROOT_EARG;
  return root_file_read (root_running_task->fds[fd], buf, size);
}

static root_ssize_t
root_write (int fd, const char *buf, root_size_t size)
{
  if (root_running_task == NULL || (root_size_t) fd >= root_running_task->nfds
      || root_running_task->fds[fd] == NULL)
    return ROOT_EARG;
  return root_file_write (root_running_task->fds[fd], buf, size);
}

static int
root_ioctl (int fd, int op, ...)
{
  int result;
  va_list args;
  if (root_running_task == NULL || (root_size_t) fd >= root_running_task->nfds)
    {
      root_seterrno (ROOT_EARG);
      return -1;
    }
  va_start (args, op);
  result = root_file_ioctlv (root_running_task->fds[fd], op, args);
  va_end (args);
  return result;
}

#endif
