#include "include/tasking.h"
#include <stddef.h>

Task tasks[MAX_TASKS];
int current_task = -1;

// Allocate stacks for tasks
uint8_t task_stacks[MAX_TASKS][4096];

// Assembly routine to switch stacks/eip
extern "C" void switch_task(uint32_t* old_esp, uint32_t new_esp, uint32_t* old_eip, uint32_t new_eip);

void init_tasking() {
    for (int i=0; i<MAX_TASKS; ++i) tasks[i].state = TASK_UNUSED;
    current_task = -1;
}

int create_task(void (*entry)(void)) {
    for (int i=0; i<MAX_TASKS; ++i) {
        if (tasks[i].state == TASK_UNUSED) {
            tasks[i].esp = (uint32_t)&task_stacks[i][4096];
            tasks[i].eip = (uint32_t)entry;
            tasks[i].state = TASK_READY;
            return i;
        }
    }
    return -1;
}

void yield() {
    int prev = current_task;
    int next = (current_task + 1) % MAX_TASKS;
    for (int i=0; i<MAX_TASKS; ++i) {
        int idx = (next + i) % MAX_TASKS;
        if (tasks[idx].state == TASK_READY) {
            if (prev >= 0 && tasks[prev].state != TASK_UNUSED)
                tasks[prev].state = TASK_READY;
            tasks[idx].state = TASK_RUNNING;
            current_task = idx;
            // Save current esp/eip, switch to next
            asm volatile (
                "mov %%esp, %0\n"
                "mov $1f, %1\n"
                "push %2\n"
                "push %3\n"
                "call switch_task\n"
                "1:\n"
                : "=m" (tasks[prev].esp), "=m" (tasks[prev].eip)
                : "r" (tasks[idx].esp), "r" (tasks[idx].eip)
            );
            break;
        }
    }
}