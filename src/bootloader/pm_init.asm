[bits 16]
sw_protected_mode: ; Switch to 32 bit protected mode
    cli ; disable interrupts
    lgdt [gdt_descriptor]
    mov eax,cr0
    or eax,0x1
    mov cr0,eax ; set first bit of cr0, this sets the cpu in 32 bit mode
    ;jmp $
    jmp CODE_SEG:protected_mode_init   ; far jump into the code segment
                                        ; far jump makes sure all instructions in pipeline are completed so as not to run instructions intended for real mode in protected mode

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
    ;jmp 0820h:7E00h
    ;jmp 0x1000
