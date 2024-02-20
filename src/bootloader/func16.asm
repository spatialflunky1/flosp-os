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

; Registers to stack save: ax,dx
; disk_load(bx:memory_offset(addr), al:read_count, dl:drive_num, es:memory_segment(addr), cl:start_sector)
disk_load:
    push dx
    push ax
    mov ah,02h ; read sectors from drive
    ; set location to read
    mov dh,0 ; read 0th head
    mov ch,0 ; read 0th cylinder
    int 13h
    jc dskerr ; carry flag indicates general fault
    mov dl,al ; dl = al (bytes read)
    pop ax    ; al = bytes supposed to have been read
    cmp al,dl
    jne dskerr
    pop dx
    ret
    dskerr:
        mov bx,newline_16
        call print_16
        mov bx,disk_read_error_msg
        call print_16
        jmp $
