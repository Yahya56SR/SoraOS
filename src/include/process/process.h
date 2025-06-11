#pragma once
#include <stddef.h>
#include <stdint.h>

namespace Kernel {

class Process {
public:
    enum State {
        CREATED,
        READY,
        RUNNING,
        BLOCKED,
        TERMINATED
    };

    Process(const char* name);
    ~Process();

    bool allocate_memory(size_t code_size, size_t data_size, 
                        size_t bss_size, size_t stack_size);

    // Getters/Setters
    const char* get_name() const { return name; }
    uint32_t get_pid() const { return pid; }
    State get_state() const { return state; }
    void set_state(State new_state) { state = new_state; }
    void set_entry_point(uint32_t entry) { entry_point = entry; }

    void* get_code_ptr() const { return (void*)code_start; }
    void* get_data_ptr() const { return (void*)data_start; }
    void* get_bss_ptr() const { return (void*)bss_start; }
    void* get_stack_ptr() const { return (void*)stack_start; }

private:
    char name[32];
    uint32_t pid;
    State state;
    
    uint32_t entry_point;
    uint32_t code_start;
    uint32_t code_size;
    uint32_t data_start;
    uint32_t data_size;
    uint32_t bss_start;
    uint32_t bss_size;
    uint32_t stack_start;
    uint32_t stack_size;
    uint32_t kernel_esp;

    friend class ProcessManager;
};

} // namespace Kernel