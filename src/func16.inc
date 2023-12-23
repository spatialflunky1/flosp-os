; Registers to stack save: si (bx is unmodified)
; print(bx:message(m8))
print: 
    push si
    mov si,bx ; can't dereference bx for some reason
    mov ah,0Eh ; tty output
    mov bh,00h ; page number
print_loop:
    mov al,[si] ; character
    int 10h
    inc si
    cmp byte [si],0
    jne print_loop
    ; loop exit
    pop si
    ret
    
