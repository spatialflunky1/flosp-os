; Registers to stack save: eax, ecx
; detect_64()
detect_64:
    push eax
    push edx
    ; Detect CPUID instruction
    ; attempt to flip the ID bit (bit 21)
    pushfd
    pop eax ; eax = flags
    mov ecx,eax ; ecx = copy of flags (for comparison)
    xor eax,1 << 21
    push eax
    popfd ; set flags
    pushfd
    pop eax ; eax = flags after bit flip
    push ecx
    popfd ; restore old flags incase the bit was flipped
    cmp eax,ecx
    je noCPUID
    ; Detect 64-bit long mode
    mov eax,0x80000000
    cpuid
    cmp eax,0x80000001
    jl noLongMode ; fail if extended functions not available
    mov eax,0x80000001
    cpuid
    test edx,1 << 29 
    jz noLongMode ; fail if LM-bit is set
    pop edx
    pop eax
    ret
    noCPUID:
        mov ebx,no_cpuid_msg
        call print_32
        jmp halt32
    noLongMode:
        mov ebx,no_long_mode_msg
        call print_32
        jmp halt32
