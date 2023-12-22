%include "boot.inc"

mov bx,message
mov cx,message_len
call print

halt: hlt

; Functions (after hlt so they wont start executing after main instructions end)
%include "func16.inc"

; Data section
message db "Hello World",0Dh,0Ah
message_len EQU $ - message ; add `align 2` before this macro if more data is added

; Magic section
times 510-($-$$) db 0
dw 0xaa55
