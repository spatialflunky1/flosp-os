%include "boot.inc"

mov bx,bootmsg
call print
mov bx,newline
call print

mov bx,4230
call print_int

mov bx,newline
call print

halt: hlt ; better idle loop
jmp halt

; Functions (after hlt so they wont start executing after main instructions end)
%include "func16.inc"

; Data section
bootmsg db "Booting Flosp...",0
align 2
newline db 0Dh,0Ah,0
; Macros

; Boot section
times 510-($-$$) db 0 ; padding to fill to 512 bytes
dw 0xaa55 ; boot num
