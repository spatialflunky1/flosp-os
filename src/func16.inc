; Registers to stack save: cx, si (bx is unmodified)
; print(bx:message(m8), cx:message_len(i16/r16))
print:
    push cx
    push si
    mov si,bx ; can't dereference bx for some reason
    mov ah,0Eh ; tty output
    mov bh,00h ; page number
print_loop:
    mov al,[si] ; character
    int 10h
    inc si
    dec cx
    cmp cx,0
    jne print_loop
    ; loop exit
    pop si
    pop cx
    ret
    
