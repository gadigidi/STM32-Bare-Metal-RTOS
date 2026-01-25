#ifndef TASKS_H_
#define TASKS_H_

#define TASKS_NUM               3 //User should change this
#define FIRST_TASK              0

typedef void (*task_entry_t) (void *arg);
extern task_entry_t task_entry [TASKS_NUM];

typedef void * task_arg_t;
extern task_arg_t task_arg[TASKS_NUM];

void idle_task(void *arg);

#endif /* TASKS_H_ */
