#include "interrupts.h"

namespace Kernel {

IDTEntry InterruptManager::idt[256];
IDTPointer InterruptManager::idt_ptr;

void InterruptManager::initialize() {
    // Set up IDT pointer
    idt_ptr.limit = sizeof(IDTEntry) * 256 - 1;
    idt_ptr.base = (uint32_t)&idt;

    // Clear IDT
    for (int i = 0; i < 256; i++) {
        // Initially set all interrupts to use the default handler
        set_handler(i, isr_stub);
    }

    // Install exception handlers
    for (int i = 0; i < 32; i++) {
        set_handler(i, (void(*)())isr_stub_table[i]);
    }

    // Load IDT
    asm volatile("lidt %0" : : "m"(idt_ptr));
}

void InterruptManager::set_handler(uint8_t vector, void (*handler)()) {
    uint32_t handler_addr = (uint32_t)handler;
    idt[vector].offset_low = handler_addr & 0xFFFF;
    idt[vector].selector = 0x08; // Kernel code segment
    idt[vector].zero = 0;
    idt[vector].type_attr = 0x8E; // Present, Ring 0, 32-bit Interrupt Gate
    idt[vector].offset_high = (handler_addr >> 16) & 0xFFFF;
}

} // namespace Kernel

// C++ interrupt handler
extern "C" void handle_interrupt(uint32_t esp) {
    // Get interrupt number and error code from stack
    uint32_t int_no = *((uint32_t*)(esp + 32));
    uint32_t err_code = *((uint32_t*)(esp + 36));

    // Handle specific interrupts
    switch(int_no) {
        case 0x0E: // Page Fault
            uint32_t fault_addr;
            asm volatile("mov %%cr2, %0" : "=r" (fault_addr));
            // Handle page fault
            break;
        case 0x0D: // General Protection Fault
            // Handle GPF
            break;
        case 0x08: // Double Fault
            // Handle double fault
            break;
        default:
            // Handle other interrupts
            break;
    }
}