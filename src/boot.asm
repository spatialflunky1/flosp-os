%include "init.asm"

mov [BOOT_DRIVE],dl ; bios stores boot drive in dl at boot

; Print boot message
mov bx,bootmsg
call print
mov bx,newline
call print
mov bx,newline
call print

;mov al,5 ; Load 5 sectors
;mov bx,0x9000 ; Load sectors to this offset from 0x0000 (ES)
;mov dl,[BOOT_DRIVE]
;call disk_load

call sw_protected_mode

halt16: hlt ; better idle loop
jmp halt16

; After hlt so they wont start executing after main instructions end
%include "func16.asm" ; 16 bit functions used by the bootloader
%include "gdt.asm" ; Global Descriptor Table
%include "pm_init.asm" ; Protected Mode initialization

; Protected mode section
[bits 32]
protected_mode_begin:
    halt32: hlt
    jmp halt32

; Data section
bootmsg db "Booting Flosp...",0
align 2
newline db 0Dh,0Ah,0
align 2
disk_read_error_msg db "Disk read error",0
align 2
BOOT_DRIVE db 0
; Macros
data_count EQU 5

; Boot section
times 510-($-$$) db 0 ; padding to fill to 512 bytes
dw 0xaa55 ; boot num

