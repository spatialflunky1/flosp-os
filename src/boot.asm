%include "boot.inc"

halt: hlt

; Functions (after hlt so they wont start executing after main instructions end)


; Data section
message db "Hello World",0Dh,0Ah,0

; Magic section
times 510-($-$$) db 0
dw 0xaa55
