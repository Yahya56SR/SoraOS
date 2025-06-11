; Task switching assembly code
global switch_task
global enter_task

section .text

; void switch_task(uint32_t* save_esp, uint32_t new_esp, uint32_t* save_eip, uint32_t new_eip);
switch_task:
    ; [esp+4]  = save_esp (ptr)
    ; [esp+8]  = new_esp (value)
    ; [esp+12] = save_eip (ptr)
    ; [esp+16] = new_eip (value)
    mov eax, [esp + 4]
    mov [eax], esp         ; Save current ESP
    mov eax, [esp + 12]
    mov dword [eax], switch_task_return  ; Save return address
    mov esp, [esp + 8]     ; Load new ESP
    mov eax, [esp + 16]
    jmp eax                ; Jump to new task
switch_task_return:
    ret

; void enter_task(uint32_t eip, uint32_t esp);
enter_task:
    mov esp, [esp + 8]     ; Load new ESP
    mov eax, [esp + 4]     ; Get entry point
    jmp eax                ; Jump to task