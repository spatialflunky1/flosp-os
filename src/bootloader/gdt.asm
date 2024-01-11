; Global Descriptor Table
gdt_start:

gdt_null: ; null descriptor, raise an exception when addressing with this value
    dd 0x0
    dd 0x0
gdt_code: ; code segment descriptor
    ; base  = 0x0
    ; limit = 0xffff
    ; 1st flags : 1001b
    ;     (1: present) (00: privilege) (1: descriptor type)
    ; type flags: 1010b
    ;     (1: code) (0: conforming) (1: readable) (0: accessed)
    ; 2nd flags : 1100b
    ;     (1: granularity) (1: 32-bit default) (0: 64-bit segment) (0: AVL/availiable for system use)
    dw 0xffff    ; limit (bits 0-15)
    dw 0x0       ; base  (bits 0-15)
    db 0x0       ; base  (bits 16-23)
    db 10011010b ; 1st flags + type flags
    db 11001111b ; 2nd flags + limit (FF/1111, bits 16-19)
    db 0x0       ; base (bits 23-31)
gdt_data: ; data segment descriptor
    ; base  = 0x0
    ; limit = 0xffff
    ; 1st flags : 1001b
    ;     (1: present) (00: privilege) (1: descriptor type)
    ; type flags: 0010b
    ;     (0: code) (0: expand down) (1: writable) (0: accessed)
    ; 2nd flags : 1100b
    ;     (1: granularity) (1: 32-bit default) (0: 64-bit segment) (0: AVL/availiable for system use)
    dw 0xffff    ; limit (bits 0-15)
    dw 0x0       ; base  (bits 0-15)
    db 0x0       ; base  (bits 16-23)
    db 10010010b ; 1st flags + type flags
    db 11001111b ; 2nd flags + limit (FF/1111, bits 16-19)
    db 0x0       ; base (bits 23-31)

gdt_end: ; start and end label used in calculations

gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; size of the GDT, 1 less than true size
    dd gdt_start               ; start address of the GDT

; Define constants to store the segment values required for protected mode
CODE_SEG EQU gdt_code - gdt_start
DATA_SEG EQU gdt_data - gdt_start
