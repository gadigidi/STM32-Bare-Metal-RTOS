#ifndef TASKS_H_
#define TASKS_H_

#define USER_TASKS_NUM          2 //User should change this
#define FIRST_TASK              0

typedef void (*task_entry_t) (void *arg);
extern task_entry_t task_entry [USER_TASKS_NUM];

typedef void * task_arg_t;
extern task_arg_t task_arg[USER_TASKS_NUM];

void os_idle_task(void *arg);
void os_error_task(void *arg);

#endif /* TASKS_H_ */
