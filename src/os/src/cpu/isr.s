; Interrupt Service Routines
[bits 64]
extern exception_handler
extern irq_handler

; Call the exception_handler on an error interrupt
%macro isr_err_stub 1
isr_stub_%+%1:
            ; error code is auto pushed here
    push 1  ; indicate error
    push %1 ; indicate vector
    push rdi
    mov rdi,rsp
    call exception_handler
    mov rsp,rax
    pop rdi
    add rsp,16 ; advance the stack by 2
    iretq
%endmacro
; Call the exception_handler on a non-error interrupt
%macro isr_no_err_stub 1
isr_stub_%+%1:
    push 0  ; dummy error code
    push 0  ; indicates non-error
    push %1 ; indicates vector
    push rdi
    mov rdi,rsp
    call exception_handler
    mov rsp,rax
    pop rdi
    add rsp,16 ; advance the stack by 2
    iretq
%endmacro
; Call the irq_handler on an irq interrupt
%macro irq_stub 1
irq_stub_%+%1:
    push %1
    push rdi
    mov rdi,rsp
    call irq_handler
    mov rsp,rax
    pop rdi
    add rsp,16 ; advance the stack by 2
    iretq
%endmacro

; All of these below are expanded by the macros above into their respective isr_stubs
isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31
irq_stub        0
irq_stub        1
irq_stub        2
irq_stub        3
irq_stub        4
irq_stub        5
irq_stub        6
irq_stub        7

global isr_stub_table

isr_stub_table:
%assign i 0 ; Assembly compile time for loop to repeat lines
%rep 32
    dq isr_stub_%+i
%assign i i+1
%endrep
; IRQs
%assign i 0
%rep 8
    dq irq_stub_%+i
%assign i i+1
%endrep
