#include "include/tasking.h"

Task tasks[MAX_TASKS];
int current_task = -1;

uint8_t task_stacks[MAX_TASKS][4096];

extern "C" void switch_task(uint32_t* save_esp, uint32_t new_esp, uint32_t* save_eip, uint32_t new_eip);
extern "C" void enter_task(uint32_t eip, uint32_t esp);

// Function to handle task completion/exit
void task_exit() {
    // Mark current task as unused
    if (current_task >= 0) {
        tasks[current_task].state = TASK_UNUSED;
    }
    // Force a yield to the next task
    yield();
    // We should never return here, but just in case
    while(1) {
        asm volatile("hlt");
    }
}

void init_tasking() {
    for (int i = 0; i < MAX_TASKS; ++i) tasks[i].state = TASK_UNUSED;
    current_task = -1;
}

int create_task(void (*entry)(void)) {
    for (int i = 0; i < MAX_TASKS; ++i) {
        if (tasks[i].state == TASK_UNUSED) {
            uint32_t* stack = (uint32_t*)&task_stacks[i][4096];
            *(--stack) = (uint32_t)task_exit;  // Push task_exit as return address
            tasks[i].esp = (uint32_t)stack;
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
    for (int i = 0; i < MAX_TASKS; ++i) {
        int idx = (next + i) % MAX_TASKS;
        if (tasks[idx].state == TASK_READY) {
            if (prev >= 0 && tasks[prev].state != TASK_UNUSED)
                tasks[prev].state = TASK_READY;
            tasks[idx].state = TASK_RUNNING;
            current_task = idx;
            // If this is the first switch to this task
            if (prev < 0) {
                enter_task(tasks[idx].eip, tasks[idx].esp);
            } else {
                // Normal context switch
                switch_task(&tasks[prev].esp, tasks[idx].esp,
                          &tasks[prev].eip, tasks[idx].eip);
            }
            break;
        }
    }
}