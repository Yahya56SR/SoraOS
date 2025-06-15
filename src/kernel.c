#include "include/io.h"
#include "include/keyboard.h"
#include "include/paging.h"
#include "include/tasking.h"
#include "include/memorys.h" // changed from kernel_util.h to memorys.h
#include "include/process/process.h"
#include "include/process/process_manager.h"
#include "include/interrupts.h"
#include "include/vectors.h" // Include the new vectors.h
#include "include/liballoc.h"
#include <stddef.h> // For NULL
#include <stdint.h>

extern uint32_t end; // Defined in linker.ld

// Multiboot info structure
struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    uint8_t color_info[6];
};

// Memory map entry structure
struct mmap_entry {
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    uint32_t type;
} __attribute__((packed));

// External variable from boot.asm
extern uint32_t mboot_info_ptr;

// Function to get total RAM in MB
uint32_t get_total_ram_mb(struct multiboot_info *mbi) {
    // Check if memory info is available (bit 0 of flags)
    if (!(mbi->flags & 0x1)) {
        return 0; // Memory info not available
    }

    // Basic memory info from multiboot
    uint32_t mem_kb = mbi->mem_lower + mbi->mem_upper;
    uint32_t mem_mb = mem_kb / 1024;

    // If memory map is available, we can get more accurate information
    if (mbi->flags & 0x40) {
        uint32_t mem_mb = 0;
        struct mmap_entry *entry = (struct mmap_entry *)mbi->mmap_addr;
        struct mmap_entry *end = (struct mmap_entry *)((uint32_t)mbi->mmap_addr + mbi->mmap_length);

        while (entry < end) {
            // Type 1 indicates available RAM
            if (entry->type == 1) {
                mem_mb += (entry->len / 1048576); // Convert bytes to MB
            }
            // Go to next entry
            entry = (struct mmap_entry *)((uint32_t)entry + entry->size + 4);
        }
        return mem_mb;
    }

    return mem_mb;
}

void kernel_main() {
    setup_paging();
    init_interrupts();
    init_tasking();

    // Example usage of the new concat_strings function
    const char* firstName = "Sora";
    const char* space = " ";
    const char* lastName = "OS";
    const char* stageHandle = " - Alpha";
    const char* lifeStage = " (pre-pre-alpha)";

    char combinedNameBuffer[100]; // Allocate a buffer large enough to hold the result
    const char* names[] = {firstName, space, lastName};
    size_t combinedNameLength = concat_strings(combinedNameBuffer, sizeof(combinedNameBuffer), names, 3);

    char finalResultBuffer[200]; // Allocate a buffer large enough
    const char* allParts[] = {combinedNameBuffer, stageHandle, lifeStage};
    size_t finalResultLength = concat_strings(finalResultBuffer, sizeof(finalResultBuffer), allParts, 3);

    print_string(finalResultBuffer, VGA_COLOR_LIGHT_GREY);

    char inputBuffer[256];
    input(inputBuffer, sizeof(inputBuffer), VGA_COLOR_LIGHT_GREY);
    print_string("You entered: ", VGA_COLOR_LIGHT_GREY);
    print_string(inputBuffer, VGA_COLOR_LIGHT_GREY);

    while (1) {}
}