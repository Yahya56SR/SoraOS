#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <stddef.h>

// Define the maximum length of a process name
#define PROCESS_NAME_MAX_LENGTH 32

// Define the possible states of a process
typedef enum {
    PROCESS_STATE_READY,
    PROCESS_STATE_RUNNING,
    PROCESS_STATE_BLOCKED,
    PROCESS_STATE_TERMINATED
} ProcessState;

// Define the Process structure
typedef struct Process {
    uint32_t id;             // Process ID
    char name[PROCESS_NAME_MAX_LENGTH]; // Process name
    ProcessState state;       // Process state
    void* stack;            // Pointer to the process's stack
    size_t stack_size;      // Size of the process's stack
} Process;

// Function prototypes
Process* process_create(uint32_t id, const char* process_name);
void process_set_stack(Process* process, void* stack, size_t stack_size);
uint32_t process_get_id(const Process* process);
const char* process_get_name(const Process* process);
void process_set_state(Process* process, ProcessState state);
ProcessState process_get_state(const Process* process);
void process_destroy(Process* process);

#endif