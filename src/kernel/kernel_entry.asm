[bits 64]
[extern main]

call main

halt_b: hlt 
jmp halt_b
