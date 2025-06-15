#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include "process.h"
#include <stddef.h>
#include <stdint.h>

void process_manager_initialize();
Process* process_manager_create_process(const char* name);
void process_manager_destroy_process(Process* process);
void process_manager_switch_to_user_mode(Process* process);
Process* process_manager_get_current_process();
void process_manager_schedule();
void process_manager_yield();

#endif