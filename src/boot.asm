%include "boot.inc"

mov bx,bootmsg
mov cx,bootmsg_len
call print

halt: hlt ; better idle loop
jmp halt

; Functions (after hlt so they wont start executing after main instructions end)
%include "func16.inc"

; Data section
bootmsg db "Booting Flosp...",0Dh,0Ah
bootmsg_len EQU $ - bootmsg ; add `align 2` before this macro if more data is added

; Magic section
times 510-($-$$) db 0
dw 0xaa55
