[GLOBAL keyboard_interrupt_handler]
[EXTERN keyboard_handler]  ; Use simple name since we'll declare it extern "C"

keyboard_interrupt_handler:
    pushad          ; Save all registers
    cld             ; Clear direction flag
    call keyboard_handler
    popad           ; Restore all registers
    iret            ; Return from interrupt