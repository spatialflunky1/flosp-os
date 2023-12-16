org 0x7c00

mov ah,00h
mov al,02h
int 10h

mov si,message
; mov ah,0Ah ; "Write character only at cursor position"
mov bh,00h ; Page number
mov cx,01h ; Print character once
mov dh,00h ; First row
mov dl,00h ; First column

print:
    ; Move cursor
    mov ah,02h
    int 10h
    inc dl

    ; Print character
    mov ah,0Ah
    mov al,[si]
    int 10h
    inc si

    cmp byte [si],0
    jne print

; Move to begining of next row
mov ah,02h
mov dh,01h
mov dl,00h 
int 10h

jmp $

message db "Hello World",0

times 510-($-$$) db 0
dw 0xaa55
