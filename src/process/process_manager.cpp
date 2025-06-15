#include "process/process_manager.h"
#include "tasking.h"
#include <stddef.h>

namespace Kernel {

void ProcessManager::initialize() {
    for (size_t i = 0; i < MAX_PROCESSES; i++) {
        processes[i] = nullptr;
    }
    current_process = nullptr;
    next_pid = 1;
}

Process* ProcessManager::create_process(const char* name) {
    // Find free slot
    size_t slot = MAX_PROCESSES;
    for (size_t i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i] == nullptr) {
            slot = i;
            break;
        }
    }
    
    if (slot == MAX_PROCESSES) {
        return nullptr; // No free slots
    }
    
    // Create new process
    Process* process = new Process(name);
    if (!process) {
        return nullptr;
    }
    
    process->pid = next_pid++;
    process->state = Process::CREATED;
    
    // Allocate memory for the process
    if (!allocate_process_memory(process)) {
        delete process;
        return nullptr;
    }
    
    processes[slot] = process;
    return process;
}

void ProcessManager::destroy_process(Process* process) {
    if (!process) return;
    
    // Find process in array
    for (size_t i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i] == process) {
            free_process_memory(process);
            delete process;
            processes[i] = nullptr;
            if (current_process == process) {
                current_process = nullptr;
            }
            break;
        }
    }
}

bool ProcessManager::start_process(Process* process) {
    if (!process || process->state != Process::CREATED) {
        return false;
    }
    
    process->state = Process::READY;
    switch_to_user_mode(process);
    return true;
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
    size_t start = current_process->pid % MAX_PROCESSES;
    for (size_t i = 0; i < MAX_PROCESSES; i++) {
        size_t idx = (start + i) % MAX_PROCESSES;
        if (processes[idx] && processes[idx]->state == Process::READY) {
            Process* next = processes[idx];
            current_process->state = Process::READY;
            next->state = Process::RUNNING;
            Process* prev = current_process;
            current_process = next;
            
            // Call task switcher
            ::yield(); // Call global yield from tasking.h
            break;
        }
    }
}

void ProcessManager::yield() {
    ::yield(); // Call global yield from tasking.h
}

bool ProcessManager::allocate_process_memory(Process* process) {
    // Simple memory allocation - you should implement proper memory management
    static uint32_t next_address = 0x100000; // Start at 1MB
    
    const size_t code_size = 64 * 1024;  // 64KB for code
    const size_t data_size = 64 * 1024;  // 64KB for data
    const size_t stack_size = 64 * 1024; // 64KB for stack
    
    process->code_start = next_address;
    process->code_size = code_size;
    next_address += code_size;
    
    process->data_start = next_address;
    process->data_size = data_size;
    next_address += data_size;
    
    process->stack_start = next_address;
    process->stack_size = stack_size;
    next_address += stack_size;
    
    // Align next_address to 4KB boundary
    next_address = (next_address + 0xFFF) & ~0xFFF;
    
    return true;
}

void ProcessManager::free_process_memory(Process* process) {
    // Should implement proper memory deallocation
    // For now, we just mark the process as terminated
    process->state = Process::TERMINATED;
}



} // namespace Kernel