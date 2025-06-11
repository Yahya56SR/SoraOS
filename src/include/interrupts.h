#pragma once
#include <stdint.h>

namespace Kernel {

// IDT entry structure
struct IDTEntry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_high;
} __attribute__((packed));

// IDT pointer structure
struct IDTPointer {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

class InterruptManager {
public:
    static void initialize();
    static void set_handler(uint8_t vector, void (*handler)());

private:
    static IDTEntry idt[256];
    static IDTPointer idt_ptr;
};

} // namespace Kernel

// Declare ISR handlers
extern "C" {
    // Array of function pointers to ISR handlers
    extern void* isr_stub_table[32];
    void isr_stub();
    void handle_interrupt(uint32_t esp);
}