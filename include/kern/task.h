#ifndef ROOT_TASK_H
#define ROOT_TASK_H

#include <stddef.h>

#include "file.h"
#include "types.h"

typedef enum
{
  ROOT_TASK_STATE_RUNNING,
  ROOT_TASK_STATE_READY,
  ROOT_TASK_STATE_BLOCKED
} root_task_state_t;

typedef struct root_task_t
{
  root_task_state_t state;
  int errno;
  root_size_t nfds;
  root_file_t **fds;
  struct root_task_t *next;
} root_task_t;

extern volatile root_task_t *root_running_task;

#endif
