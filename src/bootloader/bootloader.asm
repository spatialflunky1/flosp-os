%include "init.asm"

mov [BOOT_DRIVE],dl ; bios stores boot drive in dl at boot

; Print boot message
mov bx,bootmsg
call print_16

mov al,2 ; Load 2 sectors
mov bx,0x7E00 ; 0x0200 after 0x7C00 which is where the first sector autoloads to
;Load sectors to 0x0000(ES) : 0x7E00(BX) 
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
times 510-($-$$) db 0 ; padding to fill to 512 bytes, for aligning the definition below
dw 0xAA55 ; boot num

sector_load:
    mov ebx,protected_mode_msg
    call print_32
    mov ebx,newline
    call print_32
    ; detect 64 bit capability
    ; if not available, print error then halt
    ; if available, continue into long mode
    call detect_64
    ; returns here if long mode is detected
    mov ebx,cpu_64_detected_msg
    call print_32
    mov ebx,newline
    call print_32
    ; enable A20 line to enable the usage of all memory
    call enable_a20
    mov ebx,a20_enabled_msg
    call print_32
    ; switch to long mode
    jmp sw_long_mode

    halt32: hlt
    jmp halt32

%include "func32.asm" ; 32 bit functions
%include "gdt64.asm"
%include "lm_init.asm"
;%include "func64.asm"

[bits 64]
long_mode_begin:
    halt64: hlt
    jmp halt64

; Data section
; -------------
; Messages:
protected_mode_msg      db "Entered protected mode",0
align 2
cpu_64_detected_msg     db "64 bit processor detected",0
align 2
a20_enabled_msg         db "A20 line enabled",0
align 2
long_mode_msg           db "Entered long mode",0
; Error Messages:
align 2
no_cpuid_msg            db "FATAL: CPUID instruction not available",0
align 2
no_long_mode_msg        db "FATAL: Processor does not support 64-bit mode",0
align 2
no_a20_msg              db "FATAL: Failed to enable A20 line",0
; Internal Strings:
align 2
newline                 db 0Ah,0
; Bootloader Variables:
VIDEO_LOC dd VIDEO_MEM+(80*2)
LINE_LOC db 1 ; current line
align 2
CURSOR_LOC dw 80 ; starts at 80 to offset original boot message
; Macros
VIDEO_MEM EQU 0xB8000
CURSOR_MEM EQU 0x3D4
COLLUMNS EQU 80
WF_BB EQU 0x0F ; White foreground, black background

times 1536-($-$$) db 0 ; 2 sectors padding (2*512 + initial 512)
