#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

// IDT Entry structure
typedef struct {
    uint16_t offset_low;    // Lower 16 bits of handler function address
    uint16_t selector;      // Kernel segment selector
    uint8_t zero;          // Reserved
    uint8_t type_attr;     // Type and attributes
    uint16_t offset_high;   // Higher 16 bits of handler function address
} __attribute__((packed)) IDTEntry;

// IDTR structure
typedef struct {
    uint16_t limit;        // Size of IDT - 1
    uint32_t base;         // Base address of IDT
} __attribute__((packed)) IDTPointer;

// Function declarations
void init_interrupts();
void interrupt_manager_initialize();
void interrupt_manager_set_handler(uint8_t vector, void (*handler)());

// External assembly functions
extern void isr_stub();
extern void* isr_stub_table[];
extern void load_idt(IDTPointer* idt_ptr);

#endif