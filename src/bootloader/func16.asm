[bits 16]
; Registers to stack save: si
; print_16(bx:message(addr))
print_16: 
    push si
    mov si,bx ; can't dereference bx for some reason
    mov ah,0Eh ; tty output
    mov bh,00h ; page number
    print_16_loop:
        mov al,[si] ; character
        int 10h
        inc si
        cmp byte [si],0
        jne print_16_loop
    pop si
    ret
   
; Registers to stack save: ax, bx, cx, dx
; print_int(bx:number)
print_int:
    push ax
    push bx
    push cx
    push dx
    mov ax,bx ; div uses ax
    mov bx,10
    mov dx,0 ; upper word/remainder must be initialized beforehand
    mov cx,0
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
    
; Registers to stack save: ax,bx,cx
; print_hex(bx:number)
print_hex:
    push ax
    push bx
    push cx
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
    pop cx
    pop bx
    pop ax
    ret

; Registers to stack save: ax,cx,dx
; disk_load(bx:memory_offset(addr), al:read_count, dl:drive_num)
disk_load:
    push dx
    push cx
    push ax
    mov ah,02h ; read sectors from drive
    ; set location to read
    mov dh,0 ; read 0th head
    mov ch,0 ; read 0th cylinder
    mov cl,2 ; read 2nd sector
    int 13h
    jc dskerr ; carry flag indicates general fault
    mov dl,al ; dl = al (bytes read)
    pop ax    ; al = bytes supposed to have been read
    cmp al,dl
    jne dskerr
    pop cx
    pop dx
    ret
    dskerr:
        mov bx,disk_read_error_msg
        call print_16
        jmp $

sw_protected_mode: ; Switch to 32 bit protected mode
    cli ; disable interrupts
    lgdt [gdt_descriptor]
    mov eax,cr0
    or eax,0x1
    mov cr0,eax ; set first bit of cr0, this sets the cpu in 32 bit mode
    jmp CODE_SEG:protected_mode_init   ; far jump into the code segment
                                        ; far jump makes sure all instructions in pipeline are completed so as not to run instructions intended for real mode in protected mode
