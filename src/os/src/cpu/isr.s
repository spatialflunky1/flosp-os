; Interrupt Service Routines
[bits 64]
extern interrupt_handler
extern timer_handler

; Call the interrupt_handler on an error interrupt
%macro isr_err_stub 1
isr_stub_%+%1:
            ; error code is auto pushed here
    push 1  ; indicate error
    push %1 ; indicate vector
    ; Save all registers
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rsp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    mov rdi,rsp
    call interrupt_handler
    mov rsp,rax
    ; Restore all registers
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rsp
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
    add rsp,24 ; advance the stack by 3
    iretq
%endmacro
; Call the interrupt_handler on a non-error interrupt
%macro isr_no_err_stub 1
isr_stub_%+%1:
    push 0  ; dummy error code
    push 0  ; indicates non-error
    push %1 ; indicates vector
    ; Save all registers
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rsp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    mov rdi,rsp
    call interrupt_handler
    mov rsp,rax
    ; Restore all registers
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rsp
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
    add rsp,24 ; advance the stack by 3
    iretq
%endmacro
; Call the irq_handler on an irq interrupt
%macro irq_stub 1
irq_stub_%+%1:
    push 0     ; dummy error code
    push 0     ; indicates non-error
    push %1+32 ; indicates vector (offset by first 32 cpu interrupts)
    ; Save all registers
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rsp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    mov rdi,rsp
    call interrupt_handler
    mov rsp,rax
    ; Restore all registers
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rsp
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
    add rsp,24 ; advance the stack by 3
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
; IRQs
irq_stub        0
irq_stub        1
irq_stub        2
irq_stub        3
irq_stub        4
irq_stub        5
irq_stub        6
irq_stub        7
irq_stub        8
irq_stub        9
irq_stub        10
irq_stub        11
irq_stub        12
irq_stub        13
irq_stub        14
irq_stub        15

global isr_stub_table

isr_stub_table:
%assign i 0 ; Assembly compile time for loop to repeat lines
%rep 32
    dq isr_stub_%+i
%assign i i+1
%endrep
; IRQs
%assign i 0
%rep 15
    dq irq_stub_%+i
%assign i i+1
%endrep
