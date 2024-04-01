extern gdt64_descriptor
extern CODE_SEG64
extern DATA_SEG64

global gdt_init_asm
gdt_init_asm:
    lgdt [gdt64_descriptor]
    ; my otherworldly version of a long jump
    mov rax,rsp
    push DATA_SEG64    ;iretq_ss
    push rax           ;iretq_rsp
    pushfq             ;iretq_flags
    push CODE_SEG64    ;iretq_cs
    push segments_init ;iretq_rip
    iretq
    segments_init:
    mov ax,DATA_SEG64
    mov ds,ax
    mov ss,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
    ret
