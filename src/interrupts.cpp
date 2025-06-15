#include "include/interrupts.h"
#include "include/keyboard.h"
#include "include/shell.h"

namespace Kernel {

// Define static members
IDTEntry InterruptManager::idt[256];
IDTPointer InterruptManager::idt_ptr;

// External assembly functions
extern "C" void load_idt(void* ptr);
extern "C" void keyboard_interrupt();
extern "C" void isr_stub();

// Use InterruptManager's outb inside the handler
static inline void port_outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Generic interrupt handler
extern "C" void handle_interrupt(uint32_t esp) {
    // Get interrupt number from stack
    uint32_t int_no = *((uint32_t*)(esp + 32));
    
    // Handle keyboard interrupt
    if (int_no == 0x21) {
        Keyboard::keyboard_handler();
    }
    
    // Send EOI if this is a hardware interrupt
    if (int_no >= 0x20 && int_no < 0x30) {
        // Send EOI to slave PIC if necessary
        if (int_no >= 0x28) {
            port_outb(0xA0, 0x20);
        }
        // Send EOI to master PIC
        port_outb(0x20, 0x20);
    }
}

void InterruptManager::initialize() {
    // Set up IDT pointer
    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (uint32_t)&idt;

    // Clear IDT
    for (int i = 0; i < 256; i++) {
        set_entry(i, (uint32_t)isr_stub, 0x08, 0x8E);
    }

    // Load IDT
    load_idt(&idt_ptr);

    // Configure PIC
    // ICW1 - Start initialization
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    
    // ICW2 - Remap IRQs
    outb(0x21, 0x20); // IRQ 0-7 → INT 0x20-0x27
    outb(0xA1, 0x28); // IRQ 8-15 → INT 0x28-0x2F
    
    // ICW3 - Setup cascading
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    
    // ICW4 - Environment info
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    
    // Mask all interrupts except keyboard (IRQ1)
    outb(0x21, ~(1 << 1));
    outb(0xA1, 0xFF);
}

void InterruptManager::set_entry(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = sel;
    idt[num].zero = 0;
    idt[num].flags = flags;
}

void InterruptManager::set_handler(uint8_t interrupt, void (*handler)()) {
    set_entry(interrupt, (uint32_t)handler, 0x08, 0x8E);
}

} // namespace Kernel