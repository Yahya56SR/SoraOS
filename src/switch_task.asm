; switch_task(uint32_t* save_esp, uint32_t new_esp, uint32_t* save_eip, uint32_t new_eip)
global switch_task
switch_task:
    mov [ecx], esp   ; save old esp
    mov esp, edx     ; load new esp
    mov [eax], ebx   ; save old eip (ebx holds return address)
    mov ebx, edi     ; load new eip
    jmp ebx