#include "process/process_manager.h"
#include "tasking.h"
#include <stddef.h>
#include <stdint.h>

// Kernel utility functions
extern "C" {
    void* kmalloc(size_t size);
    void kfree(void* ptr);
    void kmemset(void* dest, int val, size_t len);
    void kmemcpy(void* dest, const void* src, size_t len);
}

// Global operators must be outside any namespace
void* operator new(size_t size) {
    return kmalloc(size);
}

void operator delete(void* ptr) {
    kfree(ptr);
}

void* operator new(size_t, void* ptr) {
    return ptr;
}

namespace Kernel {

// Define constants
#define KERNEL_MAX_PROCESSES 64

// Static member initialization
alignas(8) static uint8_t process_memory[KERNEL_MAX_PROCESSES * sizeof(Process)];
static bool process_slots[KERNEL_MAX_PROCESSES] = {false};
Process* ProcessManager::processes[KERNEL_MAX_PROCESSES] = {nullptr};
Process* ProcessManager::current_process = nullptr;
uint32_t ProcessManager::next_pid = 1;

void ProcessManager::initialize() {
    kmemset(processes, 0, sizeof(processes));
    kmemset(process_slots, 0, sizeof(process_slots));
    current_process = nullptr;
    next_pid = 1;
}

Process* ProcessManager::create_process(const char* name) {
    // Find free slot in preallocated memory
    for (size_t i = 0; i < KERNEL_MAX_PROCESSES; i++) {
        if (!process_slots[i]) {
            Process* new_process = reinterpret_cast<Process*>(&process_memory[i * sizeof(Process)]);
            // Placement new
            new (new_process) Process(name);
            new_process->pid = next_pid++;
            new_process->state = Process::READY;
            processes[i] = new_process;
            process_slots[i] = true;
            return new_process;
        }
    }
    return nullptr;
}

void ProcessManager::destroy_process(Process* process) {
    if (!process) return;
    
    // Find process in array
    for (size_t i = 0; i < KERNEL_MAX_PROCESSES; i++) {
        if (processes[i] == process) {
            process->~Process(); // Call destructor explicitly
            process_slots[i] = false;
            processes[i] = nullptr;
            if (current_process == process) {
                current_process = nullptr;
            }
            break;
        }
    }
}

void ProcessManager::switch_to_user_mode(Process* process) {
    if (!process) return;

    current_process = process;
    process->state = Process::RUNNING;

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

Process* ProcessManager::get_current_process() {
    return current_process;
}

void ProcessManager::schedule() {
    if (!current_process) return;
    
    // Find next ready process
    size_t start = current_process->pid % KERNEL_MAX_PROCESSES;
    for (size_t i = 0; i < KERNEL_MAX_PROCESSES; i++) {
        size_t idx = (start + i) % KERNEL_MAX_PROCESSES;
        if (processes[idx] && processes[idx]->state == Process::READY) {
            Process* next = processes[idx];
            current_process->state = Process::READY;
            next->state = Process::RUNNING;
            Process* prev = current_process;
            current_process = next;
            
            // Call existing task switcher
            ::yield(); // Call global yield from tasking.h
            break;
        }
    }
}

void ProcessManager::yield() {
    ::yield(); // Call global yield from tasking.h
}

} // namespace Kernel