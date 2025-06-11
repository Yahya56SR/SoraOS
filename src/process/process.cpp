#include "process/process.h"
#include "paging.h"
#include <stddef.h>
#include <stdint.h>

// Simple string copy since we can't use string.h
static void kstrncpy(char* dest, const char* src, size_t n) {
    while (n > 0 && *src) {
        *dest++ = *src++;
        n--;
    }
    while (n > 0) {
        *dest++ = '\0';
        n--;
    }
}

namespace Kernel {

Process::Process(const char* process_name) 
    : pid(0), state(CREATED), entry_point(0),
      code_start(0), code_size(0),
      data_start(0), data_size(0),
      bss_start(0), bss_size(0),
      stack_start(0), stack_size(0),
      kernel_esp(0) {
    kstrncpy(name, process_name, sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';
}

Process::~Process() {
    // Free allocated memory pages
    if (code_start) {
        // Your memory deallocation code here
    }
    if (data_start) {
        // Your memory deallocation code here
    }
    if (stack_start) {
        // Your memory deallocation code here
    }
}

bool Process::allocate_memory(size_t code_sz, size_t data_sz, 
                            size_t bss_sz, size_t stack_sz) {
    // Integrate with your existing page allocation system
    code_size = code_sz;
    data_size = data_sz;
    bss_size = bss_sz;
    stack_size = stack_sz;

    // Allocate memory pages using your existing paging system
    // This is just a placeholder - integrate with your actual page allocation
    code_start = 0x100000;  // Example virtual address
    data_start = code_start + code_size;
    bss_start = data_start + data_size;
    stack_start = bss_start + bss_size;

    return true;
}

} // namespace Kernel