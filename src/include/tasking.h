#pragma once
#include <stdint.h>

#define MAX_TASKS 2

enum TaskState {
    TASK_UNUSED,
    TASK_READY,
    TASK_RUNNING
};

struct Task {
    uint32_t esp;
    uint32_t eip;
    TaskState state;
};

extern Task tasks[MAX_TASKS];
extern int current_task;

void init_tasking();
int create_task(void (*entry)(void));
void yield();
void task_exit();  // Add this declaration