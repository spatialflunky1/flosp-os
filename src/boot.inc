org 0x7c00 ; Sets the start of memory at the base address of the program
bits 16 ; explicitly define 16 bit mode
cli ; Clear the interupt flag, "syncronization mechanism"

; Zero the data segment
xor ax,ax ; zero ax in preparation to zero ds
mov ds,ax ; zero ds (needed on some old systems)

; Set up the stack
mov bp, 0x8000
mov sp, bp

; Initialize the video mode
mov ah,00h
mov al,02h
int 10h
