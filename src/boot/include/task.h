#ifndef ROOT_TASK_H
#define ROOT_TASK_H 1

#include "types.h"

typedef void (*root_task) (void *arg);

typedef struct root_task_t
{
  root_size_t ms, ticker;
  root_task func;
  void *arg;
} root_task_t;

void root_scheduler_tick (void);
void root_schedule_tasks (void);
root_task_t *root_register_task (root_size_t ms, root_task task, void *arg);

#endif
