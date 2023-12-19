org 0x7c00 ; Sets the start of memory at the base address of the program
bits 16 ; explicitly define 16 bit mode
cli ; Clear the interupt flag, "syncronization mechanism"
xor ax,ax ; zero ax in preparation to zero ds
mov ds,ax ; zero ds (needed on some old systems)

; Set up the stack
mov bp, 0x8000
mov sp, bp

; initialize video mode
mov ah,00h
mov al,02h
int 10h

    mov ax,0
    mov si,arr
    mov bx,arrlen
addition:
    add ax,[si]
    add si,2
    dec bx
    cmp bx,0
    jne addition
    ; ax = total
    mov cx,10 ; divisor
    mov dx,0  ; dx:ax initialize first word
stack_chars:
    div cx ; dx:ax / cx
    add dx,48 ; convert to char
    push dx
    mov dx,0 ; reset the upper word
    inc bx
    cmp ax,0
    jne stack_chars
    ; bx = char count, stack = chars
    mov cx,bx
    mov bh,00h
print:
    pop ax
    mov ah,0Eh ; can only pop ax and not al so this is needed
    int 10h
    dec cx
    cmp cx,0
    jne print
    ; print newline
    mov al,0Ah
    int 10h
    mov al,0Dh
    int 10h

halt: hlt

arr dw 1,5,3,7,4,3,11
arrlen EQU ($ - arr)/2

times 510-($-$$) db 0
dw 0xaa55
