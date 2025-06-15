#include "../include/process/process.h" // Include process.h
#include "../include/memorys.h" // Include memorys.h
#include <stdint.h>
#include <string.h> // Include string.h

// Function to create a new process
Process* process_create(uint32_t id, const char* name) {
    Process* process = (Process*)kmalloc(sizeof(Process));
    if (process == NULL) {
        return NULL;
    }

    process->id = id;
    strncpy(process->name, name, PROCESS_NAME_MAX_LENGTH - 1);
    process->name[PROCESS_NAME_MAX_LENGTH - 1] = '\0';
    process->state = PROCESS_STATE_READY;
    process->stack = NULL; // Initialize stack
    process->stack_size = 0; // Initialize stack size
    return process;
}

// Function to set the stack for a process
void process_set_stack(Process* process, void* stack, size_t stack_size) {
    process->stack = stack;
    process->stack_size = stack_size;
}

// Function to get the ID of a process
uint32_t process_get_id(const Process* process) {
    return process->id;
}

// Function to get the name of a process
const char* process_get_name(const Process* process) {
    return process->name;
}

// Function to set the state of a process
void process_set_state(Process* process, ProcessState state) {
    process->state = state;
}

// Function to get the state of a process
ProcessState process_get_state(const Process* process) {
    return process->state;
}

// Function to destroy a process
void process_destroy(Process* process) {
    if (process->stack != NULL) {
        kfree(process->stack); // Free the stack if it was allocated
    }
    kfree(process);
}