#ifndef PROCESS_H
#define PROCESS_H

#include <stddef.h>
#include <stdint.h>

typedef struct Process Process;

typedef enum {
    CREATED,
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
} State;

struct Process {
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
};

Process* process_create(const char* process_name);
void process_destroy(Process* process);
int process_allocate_memory(Process* process, size_t code_size, size_t data_size,
                            size_t bss_size, size_t stack_size);

#endif