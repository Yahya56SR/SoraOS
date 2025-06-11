#pragma once
#include <stdint.h>

// Sets up basic identity paging and enables it.
void setup_paging();

// Optionally: expose the page directory for advanced use
extern uint32_t page_directory[1024];