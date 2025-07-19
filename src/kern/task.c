#include "kern/task.h"

static root_file_t *root_global_fds[2] = { 0 };

static root_task_t bootstrap_task = { .state = ROOT_TASK_STATE_RUNNING,
                                      .errno = 0,
                                      .nfds = 2,
                                      .fds = root_global_fds,
                                      .next = NULL };

volatile root_task_t *root_running_task = &bootstrap_task;
