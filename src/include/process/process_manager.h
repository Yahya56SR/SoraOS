#pragma once
#include "process.h"
#include "operators.h"
#include <stddef.h>
#include <stdint.h>

namespace Kernel {

class ProcessManager {
public:
    static void initialize();
    static Process* create_process(const char* name);
    static void destroy_process(Process* process);
    static bool start_process(Process* process);
    static Process* get_current_process();
    
    // Process scheduling methods
    static void switch_to_user_mode(Process* process);
    static void schedule();
    static void yield();

private:
    static const size_t MAX_PROCESSES = 64;
    static Process* processes[MAX_PROCESSES];
    static Process* current_process;
    static uint32_t next_pid;

    // Helper methods
    static bool allocate_process_memory(Process* process);
    static void free_process_memory(Process* process);
};

// Define static members
inline Process* ProcessManager::processes[MAX_PROCESSES] = {nullptr};
inline Process* ProcessManager::current_process = nullptr;
inline uint32_t ProcessManager::next_pid = 1;

} // namespace Kernel