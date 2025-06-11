#pragma once
#include <stdint.h>

#define MAX_TASKS 8

enum TaskState {
    TASK_UNUSED,
    TASK_READY,
    TASK_RUNNING
};

struct Task {
    uint32_t esp;         // stack pointer for task
    uint32_t eip;         // instruction pointer for task
    TaskState state;
    // You can add more registers if you want (eax, ebx, ...), for minimal demo these are enough
};

extern Task tasks[MAX_TASKS];
extern int current_task;
void init_tasking();
int create_task(void (*entry)(void));
void yield();