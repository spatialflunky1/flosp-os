; Macros
%define VIDEO_MEM 0xB8000
%define CURSOR_MEM 0x3D4
%define COLLUMNS 80
%define WF_BB 0x0F ; White foreground, black background
; %define KERNEL_OFFSET 0x9000 (only used with floppy loading kernel to 0x9000)

%include "init.asm"

mov [BOOT_DRIVE],dl ; bios stores boot drive in dl at boot

; Print boot message
mov bx,bootmsg
call print_16
mov bx,newline_16
call print_16
mov bx,disk_load_msg
call print_16

; Only needed for floppy booting (autoloads sectors on iso)
;-----------------------------------------------------------
;mov al,3 ; Load 3 sectors for the bootloader
;mov cl,2 ; start at 2nd sector
;mov bx,0x7E00 ; 0x0200 after 0x7C00 which is where the first sector autoloads to
;Load sectors to 0x0000(ES) : 0x7E00(BX) 
;mov dl,[BOOT_DRIVE]
;call disk_load

;mov al,15
;mov cl,5 ; start at 5th sector
;mov bx,0x9000
;mov dl,[BOOT_DRIVE]
;call disk_load
;-----------------------------------------------------------

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
align 2
disk_load_msg           db "Loading boot disk",0
; Error Messages:
align 2
disk_read_error_msg     db "FATAL: Disk read error",0
; Internal Strings:
align 2
newline_16              db 0Dh,0Ah,0
; Bootloader Variables:
align 2
BOOT_DRIVE              db 0

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
    mov ebx,newline
    call print_32
    ; switch to long mode
    jmp sw_long_mode

    halt32: hlt
    jmp halt32

%include "func32.asm" ; 32 bit functions
%include "gdt64.asm"
%include "lm_init.asm"
%include "func64.asm"

[bits 64]
long_mode_begin:
    mov rbx,long_mode_msg
    call print_64
    mov rbx,newline
    call print_64
    mov rbx,kernel_load_msg
    call print_64

    call KERNEL_OFFSET

    halt64: hlt
    jmp halt64

; Data section
; -------------
; Messages:
protected_mode_msg      db "Entered 32-bit protected mode",0
align 2
cpu_64_detected_msg     db "64 bit processor detected",0
align 2
a20_enabled_msg         db "A20 line enabled",0
align 2
long_mode_msg           db "Entered 64-bit long mode",0
align 2
kernel_load_msg         db "Loading kernel flosp-",KVER,0
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
align 2
VIDEO_LOC dd VIDEO_MEM+(80*4)
LINE_LOC db 2 ; current line
align 2
CURSOR_LOC dw 160 ; starts at 160 to offset original 2 boot messages

times 2048-($-$$) db 0 ; 3 sectors padding (3*512 + initial 512)
; Kernel is located directly after the bootloader
KERNEL_OFFSET EQU $
