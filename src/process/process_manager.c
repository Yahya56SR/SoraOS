#include "../include/process/process_manager.h"
#include "../include/tasking.h"
#include <stddef.h>
#include <stdint.h>

// Kernel utility functions
extern void *kmalloc(size_t size);
extern void kfree(void *ptr);
extern void kmemset(void *dest, int val, size_t len);
extern void kmemcpy(void *dest, const void *src, size_t len);

// Define constants
#define KERNEL_MAX_PROCESSES 64

// Static member initialization
static uint8_t process_memory[KERNEL_MAX_PROCESSES * sizeof(Process)];
static int process_slots[KERNEL_MAX_PROCESSES] = {0};
Process *processes[KERNEL_MAX_PROCESSES] = {NULL};
Process *current_process = NULL;
uint32_t next_pid = 1;

void process_manager_initialize()
{
    kmemset(processes, 0, sizeof(processes));
    kmemset(process_slots, 0, sizeof(process_slots));
    current_process = NULL;
    next_pid = 1;
}

Process *process_manager_create_process(const char *name)
{
    // Find free slot in preallocated memory
    for (size_t i = 0; i < KERNEL_MAX_PROCESSES; i++)
    {
        if (!process_slots[i])
        {
            Process *new_process = (Process *)&process_memory[i * sizeof(Process)];
            // Placement new
            new_process = process_create(next_pid, name);
            new_process->state = PROCESS_STATE_READY;
            processes[i] = new_process;
            process_slots[i] = 1;
            next_pid++;
            return new_process;
        }
    }
    return NULL;
}

void process_manager_destroy_process(Process *process)
{
    if (!process)
        return;

    // Find process in array
    for (size_t i = 0; i < KERNEL_MAX_PROCESSES; i++)
    {
        if (processes[i] == process)
        {
            process_destroy(process);
            process_slots[i] = 0;
            processes[i] = NULL;
            if (current_process == process)
            {
                current_process = NULL;
            }
            break;
        }
    }
}

void process_manager_switch_to_user_mode(Process *process)
{
    if (!process)
        return;

    process->state = PROCESS_STATE_RUNNING;
    current_process = process;

    // Save kernel stack pointer
    uint32_t kernel_stack;
    asm volatile("mov %%esp, %0" : "=r"(kernel_stack));

    // Calculate user stack
    uint32_t user_stack = (uint32_t)process->stack + process->stack_size;

    // Switch to user mode
    asm volatile(
        "movl %0, %%esp\n" // Set stack pointer
        "pushl $0x23\n"    // User data segment with RPL=3
        "pushl %1\n"       // Stack pointer
        "pushfl\n"         // Push flags
        "popl %%eax\n"
        "orl $0x200, %%eax\n" // Enable interrupts in EFLAGS
        "pushl %%eax\n"
        "pushl $0x1B\n" // User code segment with RPL=3
        "pushl %2\n"    // Entry point
        "iret\n"
        :
        : "r"(user_stack), "r"(user_stack), "r"(process->stack)
        : "eax");
}

Process *process_manager_get_current_process()
{
    return current_process;
}

void process_manager_schedule()
{
    if (!current_process)
        return;

    size_t start = current_process->id % KERNEL_MAX_PROCESSES;

    // Find next READY process
    for (size_t i = 0; i < KERNEL_MAX_PROCESSES; i++)
    {
        size_t idx = (start + i) % KERNEL_MAX_PROCESSES;
        if (processes[idx] && processes[idx]->state == PROCESS_STATE_READY)
        {
            Process *next = processes[idx];
            current_process->state = PROCESS_STATE_READY;
            next->state = PROCESS_STATE_RUNNING;
            current_process = next;
            break;
        }
    }
}

void process_manager_yield()
{
    yield(); // Call global yield from tasking.h
}