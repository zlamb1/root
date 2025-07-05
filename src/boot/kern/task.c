#include "task.h"

#define ROOT_MAX_TASKS 16

typedef struct task_scheduler_t
{
  volatile root_size_t elapsed_ticks;
  root_size_t num_tasks;
  root_task_t tasks[ROOT_MAX_TASKS];
} task_scheduler_t;

static task_scheduler_t root_scheduler = { 0 };

void
root_scheduler_tick (void)
{
  root_scheduler.elapsed_ticks++;
}

void
root_schedule_tasks (void)
{
  root_size_t ticks = root_scheduler.elapsed_ticks;
  root_scheduler.elapsed_ticks = 0;
  for (root_size_t i = 0; i < root_scheduler.num_tasks; i++)
    {
      root_task_t *task = root_scheduler.tasks + i;
      task->ticker += ticks;
      if (task->ticker >= task->ms)
        {
          task->func (task->arg);
          task->ticker = 0;
        }
    }
}

root_task_t *
root_register_task (root_size_t ms, root_task func, void *arg)
{
  root_task_t *task;
  if (root_scheduler.num_tasks + 1 >= ROOT_MAX_TASKS)
    return NULL;
  task = root_scheduler.tasks + root_scheduler.num_tasks++;
  task->ms = ms;
  task->ticker = 0;
  task->func = func;
  task->arg = arg;
  return task;
}
