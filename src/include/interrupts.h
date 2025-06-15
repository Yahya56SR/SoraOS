#pragma once
#include <stdint.h>

namespace Kernel {

// IDT structures
struct IDTEntry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed));

struct IDTPointer {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

class InterruptManager {
public:
    static void initialize();
    static void set_handler(uint8_t interrupt, void (*handler)());
    
private:
    static void set_entry(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
    static IDTEntry idt[256];
    static IDTPointer idt_ptr;
    
    // Port I/O functions
    static inline void outb(uint16_t port, uint8_t val) {
        asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
    }
    
    static inline uint8_t inb(uint16_t port) {
        uint8_t ret;
        asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
        return ret;
    }
};

} // namespace Kernel