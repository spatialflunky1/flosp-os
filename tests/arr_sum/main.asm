global _start

; rbx shall be used as the counter as linux syscalls are dumb and mess with rcx

section .data
arr db 1,5,3,7,4,3,11
arrlen EQU $ - arr

section .text
_start:
    mov rax,0 ; total
    mov rsi,arr ; array pointer
    mov rbx,arrlen
addition:
    add rax,[rsi]
    inc rsi
    dec rbx
    cmp rbx,0
    jne addition
    ; rax contains the total at this point, rbx is 0
    cmp rax,34
    je exit
    mov rcx,10
    mov rdx,0
stack_chars:
    div rcx ; rdx:rax / rcx = 
    add rdx,48
    push rdx
    mov rdx,0 ; reset the upper qword
    inc rbx
    cmp rax,0
    jne stack_chars
    ; rbx contains char count, stack contains chars, rax(rdi,rsi,rdx)
    mov rax,1 ; write syscall
    mov rdi,1
    mov rdx,1 ; only 1 char
;    add rbx,48
;    push rbx
;    mov rsi,rsp
;    syscall
print:
    mov rsi,rsp
    syscall
    add rsp,8 
    dec rbx
    cmp rbx,0
    jne print

    push 0Ah
    mov rsi,rsp
    syscall

exit:
    mov rax,60
    mov rdi,0
    syscall
