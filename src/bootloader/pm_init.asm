[bits 32]
protected_mode_init: 
    ; initialize registers and stack in protected mode
    mov ax,DATA_SEG
    mov ds,ax
    mov ss,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
    mov ebp,0x90000 ; update stack position
    mov esp,ebp
    jmp protected_mode_begin
