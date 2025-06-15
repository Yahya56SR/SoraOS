; interrupt_stubs.asm
[BITS 32]

[GLOBAL isr_stub]
[GLOBAL load_idt]
[EXTERN handle_interrupt]

section .text

; Export symbols
global isr_stub_table
global isr_stub

; Common ISR stub that calls a C handler
isr_stub:
    pusha           ; Push all registers
    
    mov ax, ds      ; Save data segment
    push eax
    
    mov ax, 0x10    ; Load kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    push esp        ; Push pointer to stack frame
    call handle_interrupt
    add esp, 4      ; Clean up stack
    
    pop eax         ; Restore data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    popa            ; Restore registers
    add esp, 8      ; Clean up error code and interrupt number
    iret            ; Return from interrupt

; Generate ISR stubs
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    push dword 0    ; Dummy error code
    push dword %1   ; Interrupt number
    jmp isr_stub
%endmacro

%macro ISR_ERRCODE 1
global isr%1
isr%1:
    push dword %1   ; Interrupt number
    jmp isr_stub
%endmacro

; Generate ISRs
ISR_NOERRCODE 0   ; Divide by zero
ISR_NOERRCODE 1   ; Debug
ISR_NOERRCODE 2   ; Non-maskable interrupt
ISR_NOERRCODE 3   ; Breakpoint
ISR_NOERRCODE 4   ; Overflow
ISR_NOERRCODE 5   ; Bound range exceeded
ISR_NOERRCODE 6   ; Invalid opcode
ISR_NOERRCODE 7   ; Device not available
ISR_ERRCODE   8   ; Double fault
ISR_NOERRCODE 9   ; Coprocessor segment overrun
ISR_ERRCODE   10  ; Invalid TSS
ISR_ERRCODE   11  ; Segment not present
ISR_ERRCODE   12  ; Stack segment fault
ISR_ERRCODE   13  ; General protection fault
ISR_ERRCODE   14  ; Page fault
ISR_NOERRCODE 15  ; Reserved
ISR_NOERRCODE 16  ; x87 FPU error
ISR_ERRCODE   17  ; Alignment check
ISR_NOERRCODE 18  ; Machine check
ISR_NOERRCODE 19  ; SIMD floating-point
ISR_NOERRCODE 20  ; Virtualization
ISR_NOERRCODE 21  ; Reserved
ISR_NOERRCODE 22  ; Reserved
ISR_NOERRCODE 23  ; Reserved
ISR_NOERRCODE 24  ; Reserved
ISR_NOERRCODE 25  ; Reserved
ISR_NOERRCODE 26  ; Reserved
ISR_NOERRCODE 27  ; Reserved
ISR_NOERRCODE 28  ; Reserved
ISR_NOERRCODE 29  ; Reserved
ISR_ERRCODE   30  ; Security
ISR_NOERRCODE 31  ; Reserved

; ISR table
section .data
align 4
isr_stub_table:
%assign i 0
%rep 32
    dd isr%+i      ; Store address of each ISR handler
%assign i i+1
%endrep

; Load IDT
load_idt:
    mov eax, [esp + 4]  ; Get pointer to IDT
    lidt [eax]         ; Load IDT
    ret