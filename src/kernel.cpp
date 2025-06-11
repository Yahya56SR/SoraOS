#include "include/paging.h"
#include "include/tasking.h"
#include "include/process/process_manager.h"
#include "interrupts.h"

// Function to clear the screen
void clear_screen() {
    uint16_t* vga_buffer = (uint16_t*)0xB8000;
    for (int i = 0; i < 80 * 25; i++) {
        vga_buffer[i] = 0x0720; // Light gray on black, space character
    }
}

// Simple print function for debugging
void kprint(const char* str, int x, int y) {
    uint16_t* vga_buffer = (uint16_t*)0xB8000;
    int offset = y * 80 + x;
    while (*str) {
        vga_buffer[offset++] = (uint16_t)*str++ | 0x0700;
    }
}

extern "C" void kernel_main() {
    // Clear screen
    clear_screen();
    kprint("SoraOS booting...", 0, 0);

    // Initialize interrupt system
    Kernel::InterruptManager::initialize();
    kprint("Interrupts initialized", 0, 1);

    // Initialize paging
    setup_paging();
    kprint("Paging initialized", 0, 2);

    // Initialize process management
    Kernel::ProcessManager::initialize();
    kprint("Process management initialized", 0, 3);

    // Initialize task system
    init_tasking();
    kprint("Task system initialized", 0, 4);

    // Enable interrupts
    asm volatile("sti");
    kprint("Interrupts enabled", 0, 5);

    // Enter kernel loop
    kprint("Entering kernel loop", 0, 6);
    while (1) {
        asm volatile("hlt");
    }
}