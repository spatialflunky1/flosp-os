; Registers to stack save: si
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
    pop si
    ret
   
; Registers to stack save: ax, bx, cx, dx
; print_int(bx:number(r16))
print_int:
    push ax
    push bx
    push cx
    push dx
    mov ax,bx ; div uses ax
    mov bx,10
    convert_int:
        div bx
        add dx,48 ; convert remainder to char
        push dx
        mov dx,0
        inc cx
        cmp ax,0
        jne convert_int
    mov bh,00h ; page num
    print_int_chars:
        pop ax
        mov ah,0Eh
        int 10h
        dec cx
        cmp cx,0
        jne print_int_chars
    pop dx
    pop cx
    pop bx
    pop ax
    ret
    
; Registers to stack save:
; print_hex(bx:number(r16))
print_hex:
    push ax
    push bx
    push cx
    push dx
    ; print 0x to signify hex
    push bx
    mov bh,00h ; page
    mov ah,0Eh
    mov al,'0'
    int 10h
    mov al,'x'
    int 10h
    pop ax ; div uses ax
    mov bx,16
    convert_hex:
        div bx
        cmp dx,10
        jge alph
numb:   add dx,48 ; convert remainder to char
        jmp exit
alph:   add dx,55
exit:   push dx
        mov dx,0
        inc cx
        cmp ax,0
        jne convert_hex
    mov bh,00h ; page num
    print_hex_chars:
        pop ax
        mov ah,0Eh
        int 10h
        dec cx
        cmp cx,0
        jne print_hex_chars
    pop dx
    pop cx
    pop bx
    pop ax
    ret
