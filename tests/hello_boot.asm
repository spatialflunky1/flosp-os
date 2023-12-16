org 0x7c00 ; Sets the start of memory at the base address of the program
bits 16 ; explicitly define 16 bit mode
cli ; Clear the interupt flag, "syncronization mechanism"
xor ax,ax ; zero ax in preparation to zero ds
mov ds,ax ; zero ds (needed on some old systems)

; Set up the stack
mov bp, 0x8000
mov sp, bp

mov ah,00h
mov al,02h
int 10h

mov si,message
mov ah,0Eh
mov bh,00h ; page number (no clue)
print:
    mov al,[si] ; Select current character
    int 10h
    inc si ; Advance character pointer
    cmp byte [si],0
    jne print

halt: hlt

message db "Hello World",0Dh,0Ah,0

times 510-($-$$) db 0
dw 0xaa55
