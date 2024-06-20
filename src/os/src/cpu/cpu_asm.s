; Assembly Related CPU Functions
global read_cr0
global write_cr0
global read_cr2
[bits 64]

read_cr0:
    ; return the value to rax
    mov rax, cr0
    ret

write_cr0:
    ; rdi contains the parameter cr0_val
    mov cr0, rdi
    ret

read_cr2:
    ; return the value to rax
    mov rax, cr2
    ret
