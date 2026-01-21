#ifndef TASKS_H_
#define TASKS_H_

#define TASKS_NUM               2 //User should change this
#define TASK_FIRST              0

typdef void (*task_entry_t) (void);
extern task_entry_t task_entry [TASKS_NUM];

typedef void * task_arg_t;
extern task_arg_t task_arg[TASKS_NUM];

#endif /* TASKS_H_ */
