[bits 32]
; Registers to stack save: esi,edx,ecx,eax 
; print_32(ebx:message(addr))
print_32:
    push esi
    push edx
    push ecx
    push eax
    mov esi,ebx
    mov eax,0 ; to initialize the upper 16 bits
    mov bl,[LINE_LOC] ; line counter
    mov edx,[VIDEO_LOC]
    mov cx,[CURSOR_LOC] ; cursor address does not change, no need to set at end of 
    print_32_loop:
        mov al,[esi] ; character
        mov ah,WF_BB ; color
        cmp al,0
        je print_32_exit
        cmp al,0Ah
        je print_32_newline
        mov [edx],ax
        add edx,2 ; Move to next char cell in memory
        inc cx
        newline_32_ret:
        inc esi
        jmp print_32_loop
    print_32_newline:
        push eax
        inc bl
        ; increase counter
        ; set vid equal to original + (bl*80*2)
        mov al,160
        mul bl
        mov edx,VIDEO_MEM
        add edx,eax
        shr ax,1
        mov cx,ax
        pop eax
        jmp newline_32_ret
    print_32_exit:
    mov [VIDEO_LOC],edx
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
    pop eax
    pop ecx
    pop edx
    pop esi
    ret

; Macros
VIDEO_MEM EQU 0xB8000
CURSOR_MEM EQU 0x3D4
COLLUMNS EQU 80
WF_BB EQU 0x0F ; White foreground, black background
; Variables
VIDEO_LOC dd VIDEO_MEM+(80*2)
LINE_LOC db 1 ; current line
align 2
CURSOR_LOC dw 80 ; starts at 80 to offset original boot message
