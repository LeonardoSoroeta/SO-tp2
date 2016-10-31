
global _get_cr3
global _set_cr3

extern os_dump_regs

section .text

%macro PUSHALL 0
    push    RBX
    push    RDI
    push    RSI
    push    RSP
    push    R12
    push    R13
    push    R14
    push    R15
%endmacro

%macro POPALL 0
    pop     R15
    pop     R14
    pop     R13
    pop     R12
    pop     RSP
    pop     RSI
    pop     RDI
    pop     RBX
%endmacro

%macro ENTER 0
    push    rbp
    mov     rbp, rsp
    PUSHALL
%endmacro

%macro LEAVE 0
    POPALL
    mov     rsp, rbp
    pop     rbp
    ret
%endmacro

_get_cr3:
    ENTER
    xor rax, rax
    mov rax, cr3
    LEAVE

_set_cr3:
    ENTER
    mov cr3, rdi
    LEAVE