[bits 32]
; print_32(ebx:message(addr))
print_32:
    push esi
    push edx
    mov esi,ebx
    mov edx,[VIDEO_LOC]
    print_32_loop:
        mov al,[esi] ; character
        mov ah,WF_BB ; color
        cmp al,0
        je print_32_exit
        cmp al,0Ah
        je print_32_newline
        mov [edx],ax
        add edx,2 ; Move to next char cell in memory
        newline_32_ret:
        inc esi
        jmp print_32_loop
    print_32_newline:
        ; newline is just add enough that the cursor is at the next line assuming char wrap in video memory
        mov edx,VIDEO_MEM
        add edx,80*2 ; each char cell is 2 bytes wide, a line is 80 collumns
        jmp newline_32_ret
    print_32_exit:
    mov [VIDEO_LOC],edx
    pop edx
    pop esi
    ret

; Macros
VIDEO_MEM EQU 0xb8000
VIDEO_LOC dd VIDEO_MEM
WF_BB EQU 0x0f ; White foreground, black background
