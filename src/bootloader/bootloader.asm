%include "init.asm"

mov [BOOT_DRIVE],dl ; bios stores boot drive in dl at boot

; Print boot message
mov bx,bootmsg
call print_16

mov al,1 ; Load 1 sector
mov bx,0x7E00 ; 0x0200 after 0x7C00 which is where the first sector autoloads to
;Load sectors to 0x0000(ES) : 0x9000(BX) 
;mov es,bx
;mov bx,0x0
mov dl,[BOOT_DRIVE]
call disk_load

jmp sw_protected_mode

halt16: hlt ; better idle loop
jmp halt16

%include "func16.asm" ; 16 bit functions for starting the bootloader
%include "gdt.asm" ; Global Descriptor Table
%include "pm_init.asm" ; Protected Mode initialization

; 2nd sector starts here
; Protected mode section
[bits 32]
protected_mode_begin:
    ; enter 2nd sector from here
    jmp 0x7E00

; Data section
; -------------
; Messages:
bootmsg                 db "Booting Flosp...",0
; Error Messages:
align 2
disk_read_error_msg     db "FATAL: Disk read error",0
; Internal Strings:
align 2
newline_16              db 0Dh,0Ah,0
; Bootloader Variables:
align 2
BOOT_DRIVE              db 0
; Macros

; Boot section
times 510-($-$$) db 0 ; padding to fill to 512 bytes
dw 0xAA55 ; boot num

sector_load:
    mov ebx,protected_mode_message
    call print_32
    mov ebx,newline
    call print_32
    ; detect 64 bit capability
    ; if not available, print error then halt
    ; if available, continue into long mode
    call detect_64
    ; returns here if long mode is detected
    halt32: hlt
    jmp halt32

%include "func32.asm" ; 32 bit functions
%include "lm_init.asm"

;[bits 64]
;long_mode_begin:
;    halt64: hlt
;    jmp halt64

; Data section
; -------------
; Messages:
protected_mode_message  db "Entered into protected mode",0
; Error Messages:
align 2
no_cpuid_msg            db "FATAL: CPUID instruction not available",0
align 2
no_long_mode_msg        db "FATAL: Processor does not support 64-bit mode",0
; Internal Strings:
align 2
newline                 db 0Ah,0
; Bootloader Variables:
; Macros

times 1024-($-$$) db 0 ; sector padding
