; Assembly Related Memory Functions
global flush_tlb
[bits 64]

flush_tlb:
    mov rax,cr3
    mov cr3,rax
