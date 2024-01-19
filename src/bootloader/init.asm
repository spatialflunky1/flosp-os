[bits 16]
org 0x7C00 ; Sets the start of memory at the base address of the program

; Set up the segments
mov ax,cs
mov ds,ax
mov es,ax
mov ss,ax

; Set up the stack
mov bp, 0x7C00
mov sp, bp

; Initialize the video mode
mov ah,00h
mov al,02h
int 10h
