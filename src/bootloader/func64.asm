[bits 64]
; Registers to stack save: rsi,rdx,rcx,rax 
; print_64(rbx:message(addr))
print_64:
    push rsi
    push rdx
    push rcx
    push rax
    mov rsi,rbx
    mov eax,0 ; to initialize the upper 16 bits
    mov bl,[LINE_LOC] ; line counter
    mov edx,[VIDEO_LOC]
    mov cx,[CURSOR_LOC] ; cursor address does not change, no need to set at end of 
    print_64_loop:
        mov al,[rsi] ; character
        mov ah,WF_BB ; color
        cmp al,0
        je print_64_exit
        cmp al,0Ah
        je print_64_newline
        mov [edx],ax
        add edx,2 ; Move to next char cell in memory
        inc cx
        newline_64_ret:
        inc rsi
        jmp print_64_loop
    print_64_newline:
        push rax
        inc bl
        ; increase counter
        ; set vid equal to original + (bl*80*2)
        mov al,160
        mul bl
        mov edx,VIDEO_MEM
        add edx,eax
        shr ax,1
        mov cx,ax
        pop rax
        jmp newline_64_ret
    print_64_exit:
    mov [VIDEO_LOC],edx
    mov [LINE_LOC],bl
    ; set cursor pos
    mov dx,CURSOR_MEM
    mov al,0x0F 
    out dx,al ; 0x3D4->0x0F sets 0x3D5 to hold the low byte
    inc dl ; dx = 0x3D5
    mov al,cl
    out dx,al ; set low byte of position
    dec dl ; dx = 0x3D4
    mov al,0x0E
    out dx,al ; 0x3D4->0x0E sets 0x3D5 to hold the high byte
    inc dl ; dx = 0x3D5
    mov al,ch
    out dx,al
    ;---------
    mov [CURSOR_LOC],cx
    pop rax
    pop rcx
    pop rdx
    pop rsi
    ret
