#include "kern/errno.h"
#include "kern/types.h"

int root_errno = ROOT_SUCCESS;

void
root_seterrno (int errno)
{
  root_errno = errno;
}
