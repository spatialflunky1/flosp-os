; Interrupt Service Routines
[bits 64]
extern exception_handler

; Call the exception_handler on an error interrupt
%macro isr_err_stub 1
isr_stub_%+%1:
    push 1 ; error code
    push %1
    push rdi
    mov rdi,rsp
    call exception_handler
    pop rdi
    add rsp,16 ; advance the stack by 2
    iretq
%endmacro
; Call the exception_handler on a non-error interrupt
%macro isr_no_err_stub 1
isr_stub_%+%1:
    push 0 ; no error code
    push %1
    push rdi
    mov rdi,rsp
    call exception_handler
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
;irq_stub        32

global isr_stub_table
isr_stub_table:
%assign i 0 ; Assembly compile time for loop to repeat lines
%rep 32
    dq isr_stub_%+i
%assign i i+1

%endrep
