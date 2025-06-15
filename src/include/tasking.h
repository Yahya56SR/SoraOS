#ifndef TASKING_H
#define TASKING_H

#include <stdint.h>

#define MAX_TASKS 2

typedef enum {
    TASK_UNUSED,
    TASK_READY,
    TASK_RUNNING
} TaskState;

typedef struct {
    uint32_t esp;
    uint32_t eip;
    TaskState state;
} Task;

extern Task tasks[MAX_TASKS];
extern int current_task;

void init_tasking();
int create_task(void (*entry)(void));
void yield();
void task_exit();  // Add this declaration

#endif