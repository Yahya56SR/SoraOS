; boot.asm - Multiboot-compliant bootloader for 32-bit mode
MBOOT_PAGE_ALIGN    equ 1<<0
MBOOT_MEM_INFO      equ 1<<1
MBOOT_HEADER_MAGIC  equ 0x1BADB002
MBOOT_HEADER_FLAGS  equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM      equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

section .multiboot
align 4
    dd MBOOT_HEADER_MAGIC        ; Magic number
    dd MBOOT_HEADER_FLAGS        ; Flags
    dd MBOOT_CHECKSUM           ; Checksum

section .bss
align 16
stack_bottom:
    resb 16384                  ; 16 KiB stack
stack_top:

section .data
align 4
gdt_start:
    ; Null descriptor
    dq 0

gdt_code:
    ; Code segment
    dw 0xFFFF       ; Limit (bits 0-15)
    dw 0            ; Base (bits 0-15)
    db 0            ; Base (bits 16-23)
    db 10011010b    ; Access byte
    db 11001111b    ; Flags + Limit (bits 16-19)
    db 0            ; Base (bits 24-31)

gdt_data:
    ; Data segment
    dw 0xFFFF       ; Limit (bits 0-15)
    dw 0            ; Base (bits 0-15)
    db 0            ; Base (bits 16-23)
    db 10010010b    ; Access byte
    db 11001111b    ; Flags + Limit (bits 16-19)
    db 0            ; Base (bits 24-31)

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; GDT size (minus 1)
    dd gdt_start                ; GDT address

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

section .text
global _start
extern kernel_main

_start:
    cli                         ; Disable interrupts
    
    ; Set up stack
    mov esp, stack_top
    
    ; Save multiboot info pointer
    push ebx
    
    ; Load GDT
    lgdt [gdt_descriptor]
    
    ; Update segment registers
    jmp CODE_SEG:.reload_cs    ; Far jump to set CS
.reload_cs:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Enable A20 line
    in al, 0x92
    or al, 2
    out 0x92, al
    
    ; Initialize core kernel features
    call kernel_main
    
    ; If kernel returns, hang the CPU
.hang:
    cli                        ; Disable interrupts
    hlt                       ; Halt the CPU
    jmp .hang                 ; Just in case