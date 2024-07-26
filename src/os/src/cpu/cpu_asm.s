; Assembly Related CPU Functions
global read_cr0
global write_cr0
global read_cr2
global read_cr3
global read_cr4
global write_cr4
[bits 64]

read_cr0:
    ; return the value to rax
    mov rax, cr0
    ret

write_cr0:
    ; rdi contains the parameter: cr0_val
    mov cr0, rdi
    ret

read_cr2:
    ; return the value to rax
    mov rax, cr2
    ret

read_cr3:
    ; return the value to rax
    mov rax, cr3
    ret

read_cr4:
    ; return the value to rax
    mov rax, cr4
    ret

write_cr4:
    ; rdi contains the parameter: cr4_val
    mov cr4, rdi
    ret