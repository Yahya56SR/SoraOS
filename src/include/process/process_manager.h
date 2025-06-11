#pragma once
#include "process.h"
#include <stddef.h>
#include <stdint.h>

namespace Kernel {

class ProcessManager {
public:
    static void initialize();
    static Process* create_process(const char* name);
    static void destroy_process(Process* process);
    static void switch_to_user_mode(Process* process);
    static Process* get_current_process();
    
    // Integration with existing task system
    static void schedule();
    static void yield();

private:
    static const size_t KERNEL_MAX_PROCESSES = 64;
    static Process* processes[KERNEL_MAX_PROCESSES];
    static Process* current_process;
    static uint32_t next_pid;

    static void setup_user_mode(Process* process);
};

} // namespace Kernel