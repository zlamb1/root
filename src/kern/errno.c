#include "errno.h"
#include "types.h"

int root_errno = ROOT_SUCCESS;

void
root_seterrno (int errno)
{
  root_errno = errno;
}
