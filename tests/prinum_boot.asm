org 0x7c00 ; Sets the start of memory at the base address of the program
bits 16 ; explicitly define 16 bit mode
cli ; Clear the interupt flag, "syncronization mechanism"
xor ax,ax ; zero ax in preparation to zero ds
mov ds,ax ; zero ds (needed on some old systems)

; Set up the stack
mov bp, 0x8000
mov sp, bp

; Set video 80x25 greyscale text mode (to clear the screen)
mov ah,00h
mov al,02h
int 10h

; Initialize counter register
mov cx,0

; Add a newline to the stack for after
push 0Dh
push 0Ah
add cx,2

; Convert the digits of the number variable to chars and add them to the stack
mov ax,[number]
mov bx,10
convnum:
    div bx
    add dx,48
    push dx
    mov dx,0
    inc cx

    cmp ax,0
    jne convnum

; Pop the digits of the number from the stack and print them
mov bh,00h ; page number
print:
    pop ax
    mov ah,0Eh
    int 10h
    dec cx
    cmp cx,0
    jne print

; Exit
halt: hlt

; Data section
number dw 12349

; Magic section
times 510-($-$$) db 0
dw 0xaa55
