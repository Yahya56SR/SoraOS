#include "include/paging.h"

// Align to 4KB boundaries as required by hardware
uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t first_page_table[1024] __attribute__((aligned(4096)));

void init_paging() {
    // Clear page directory
    for (int i = 0; i < 1024; i++) {
        page_directory[i] = 0x00000002; // Supervisor, write, not present
    }
    // Identity map first 4MB of RAM (0x00000000 - 0x003FFFFF)
    for (int i = 0; i < 1024; i++) {
        first_page_table[i] = (i * 0x1000) | 3; // Present, RW, Supervisor
    }
    // Point first entry in directory to our page table
    page_directory[0] = ((uint32_t)first_page_table) | 3;
}

void load_page_directory(uint32_t* pd) {
    asm volatile ("mov %0, %%cr3" : : "r"(pd));
}

void enable_paging() {
    uint32_t cr0;
    asm volatile ("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000; // Set PG bit
    asm volatile ("mov %0, %%cr0" : : "r"(cr0));
}

void setup_paging() {
    init_paging();
    load_page_directory(page_directory);
    enable_paging();
}