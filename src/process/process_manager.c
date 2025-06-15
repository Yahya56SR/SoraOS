#include "process/process_manager.h"
#include "tasking.h"
#include <stddef.h>
#include <stdint.h>

// Kernel utility functions
extern void* kmalloc(size_t size);
extern void kfree(void* ptr);
extern void kmemset(void* dest, int val, size_t len);
extern void kmemcpy(void* dest, const void* src, size_t len);

// Define constants
#define KERNEL_MAX_PROCESSES 64

// Static member initialization
static uint8_t process_memory[KERNEL_MAX_PROCESSES * sizeof(Process)];
static int process_slots[KERNEL_MAX_PROCESSES] = {0};
Process* processes[KERNEL_MAX_PROCESSES] = {NULL};
Process* current_process = NULL;
uint32_t next_pid = 1;

void process_manager_initialize() {
    kmemset(processes, 0, sizeof(processes));
    kmemset(process_slots, 0, sizeof(process_slots));
    current_process = NULL;
    next_pid = 1;
}

Process* process_manager_create_process(const char* name) {
    // Find free slot in preallocated memory
    for (size_t i = 0; i < KERNEL_MAX_PROCESSES; i++) {
        if (!process_slots[i]) {
            Process* new_process = (Process*)&process_memory[i * sizeof(Process)];
            //Placement new
            new_process = process_create(name);
            new_process->pid = next_pid++;
            new_process->state = READY;
            processes[i] = new_process;
            process_slots[i] = 1;
            return new_process;
        }
    }
    return NULL;
}

void process_manager_destroy_process(Process* process) {
    if (!process) return;

    // Find process in array
    for (size_t i = 0; i < KERNEL_MAX_PROCESSES; i++) {
        if (processes[i] == process) {
            process_destroy(process);
            process_slots[i] = 0;
            processes[i] = NULL;
            if (current_process == process) {
                current_process = NULL;
            }
            break;
        }
    }
}

void process_manager_switch_to_user_mode(Process* process) {
    if (!process) return;

    current_process = process;
    process->state = RUNNING;

    // Save kernel stack
    asm volatile("mov %%esp, %0" : "=r"(process->kernel_esp));

    // Set up stack for user mode
    uint32_t user_stack = process->stack_start + process->stack_size;

    // Switch to user mode
    asm volatile(
        "mov %0, %%esp\n"    // Set up user stack
        "push $0x23\n"       // User data segment
        "push %0\n"          // User stack
        "pushf\n"            // EFLAGS
        "push $0x1B\n"       // User code segment
        "push %1\n"          // Entry point
        "iret"               // Switch to user mode
        : : "r"(user_stack), "r"(process->entry_point)
    );
}

Process* process_manager_get_current_process() {
    return current_process;
}

void process_manager_schedule() {
    if (!current_process) return;

    // Find next ready process
    size_t start = current_process->pid % KERNEL_MAX_PROCESSES;
    for (size_t i = 0; i < KERNEL_MAX_PROCESSES; i++) {
        size_t idx = (start + i) % KERNEL_MAX_PROCESSES;
        if (processes[idx] && processes[idx]->state == READY) {
            Process* next = processes[idx];
            current_process->state = READY;
            next->state = RUNNING;
            Process* prev = current_process;
            current_process = next;

            // Call existing task switcher
            yield(); // Call global yield from tasking.h
            break;
        }
    }
}

void process_manager_yield() {
    yield(); // Call global yield from tasking.h
}