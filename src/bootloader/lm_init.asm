[bits 32]
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
    xor eax,1<<21
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
    test edx,1<<29 
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

; Registers to stack save: edi,esi,eax
; enable_a20()
enable_a20:
    push edi
    push esi
    push eax
    ; check if the line is already enabled before attempting to enable
    call a20_check
    ; if still in function then a20 is not set
    ; firstly the traditional method using keyboard I/O (0x64: cmd port, 0x60: data port)
    cli
    call a20_wait
    mov al,0xAD
    out 0x64,al ; disable keyboard
    call a20_wait
    mov al,0xD0
    out 0x64,al ; send command to read from input buffer
    call a20_wait2
    in al,0x60
    push eax ; save input to stack
    call a20_wait
    mov al,0xD1
    out 0x64,al ; send command to write to output buffer
    call a20_wait
    pop eax
    or al,2 ; set bit 1
    out 0x60,al ; send read input with bit 1 set to output buffer
    call a20_wait
    mov al,0xAE
    out 0x64,al ; enable keyboard
    call a20_wait
    sti
    call a20_check
    ; if this method was unsucsessful try the FAST A20 Gate
    in al,0x92
    test al,2
    jnz after_fast_a20
    or al,2
    and al,0xFE ; clear bit 0, used for fast reset
    out 0x92,al
    after_fast_a20: call a20_check
    ; reached this point if all methods failed
    mov ebx,no_a20_msg
    call print_32
    a20_exit:
        pop eax ; double pop to compensate for the call to a20_check
        pop eax
        pop esi
        pop edi
        ret
    a20_check:
        ; compare an odd Mb address to its even counterpart
        ; if a20 is not set then both addresses point to the same location
        mov edi,0x112345 ; odd address
        mov esi,0x012345 ; even address
        mov [esi],esi
        mov [edi],edi
        cmpsd ; instruction to compare values at esi and edi
        jne a20_exit
        ret
    a20_wait: ; writing 0x60 or 0x64
        ; bit 1 needs to be cleared before writing to 0x60 or 0x64
        ; wait until bit 1 is clear
        in al,0x64
        test al,2
        jnz a20_wait
        ret
    a20_wait2: ; reading 0x60
        ; wait until bit 0 is set
        ; bit 0 needs to be set before reading to 0x60
        in al,0x64
        test al,1
        jz a20_wait2
        ret

sw_long_mode:
    ; Set up paging
    ;---------------
    ; Clear the page tables
    mov edi,0x1000
    mov cr3,edi ; set the CR3 Page-level Write-Through bit (bit 3)
    xor eax,eax
    mov ecx,4096
    rep stosd ; fill 4096 DW (val=0) from 0x1000, clear mem
    mov edi,cr3
    ; set up the page tables
    mov DWORD [edi],0x2003 ; PML4T[0] (0x1000) = PDPT
    add edi,0x1000
    mov DWORD [edi],0x3003 ; PDPT[0] (0x2000) = PDT
    add edi,0x1000
    mov DWORD [edi],0x4003 ; PDT[0] (0x3000) = PT
    add edi,0x1000 ; edi = PT
    ; Identity map the first two Mb
    mov ebx,0x00000003
    mov ecx,512 ; loop counter
    set_entry:
        mov DWORD [edi],ebx ; set the first two bits in the entry (Present & RW)
        add ebx,0x1000
        add edi,8 ; advance to next entry
        loop set_entry
    ; Enable PAE-paging by setting the PAE_bit (bit 5) in CR4
    mov eax,cr4
    or eax,1<<5 
    mov cr4,eax
    ;---------------
    ; Set the LM-bit (bit 8)
    ; The LM-bit is bit 8 of the MSR (Model Specific Register) number 0xC0000080
    mov ecx,0xC0000080
    rdmsr
    or eax,1<<8
    wrmsr
    ; Enable Paging, set PG-bit (bit 31 in cr0)
    mov eax,cr0
    or eax,1<<31
    mov cr0,eax
    ; Load the 64 bit GDT and enter the 64 bit codespace
    lgdt [gdt64_descriptor]
    jmp CODE_SEG64:long_mode_init
    jmp $

[bits 64]
long_mode_init:
    mov ax,DATA_SEG64
    mov ds,ax
    mov ss,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
    jmp long_mode_begin
