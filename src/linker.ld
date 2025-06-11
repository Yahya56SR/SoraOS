ENTRY(_start)

SECTIONS {
    . = 1M;                     /* Load kernel at 1 MiB */
    
    .multiboot BLOCK(4K) : ALIGN(4K) {
        *(.multiboot)           /* Multiboot header first */
    }
    
    .text BLOCK(4K) : ALIGN(4K) {
        *(.text)                /* Code sections */
    }
    
    .rodata BLOCK(4K) : ALIGN(4K) {
        *(.rodata)              /* Read-only data */
    }
    
    .data BLOCK(4K) : ALIGN(4K) {
        *(.data)                /* Initialized data */
    }
    
    .bss BLOCK(4K) : ALIGN(4K) {
        *(COMMON)               /* Common symbols */
        *(.bss)                /* Uninitialized data */
        *(.bootstrap_stack)    /* Initial stack */
    }
    
    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.note.*)
    }
}