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
    mov dx,0 ; upper word/remainder must be initialized beforehand
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
    pop bx
    mov cx,0 ; counter
    get_hex_digits:
        mov ax,bx
        and ax,0x000F ; ax contains last 4 bits, aka single digit of hex
        cmp ax,10 ; convert hex digit to char
        jge alph
numb:   add ax,48
        jmp hdone
alph:   add ax,55
hdone:  push ax
        inc cx
        shr bx,4
        cmp bx,0
        jne get_hex_digits
    mov bh,00h   
    print_hex_digits:
        pop ax
        mov ah,0Eh
        int 10h
        dec cx
        cmp cx,0
        jne print_hex_digits
    pop dx
    pop cx
    pop bx
    pop ax
    ret
