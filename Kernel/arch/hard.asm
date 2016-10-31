global _get_memory_size
global _get_vga_address

global _get_cr3
global _set_cr3

global _get_esp
global _get_ebp

global _get_r9
global _get_r11
global _get_r12

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

_get_memory_size:
	ENTER
	xor rax, rax
	mov eax, [0x5020]
	LEAVE

_get_vga_address:
	ENTER
	xor rax, rax
	mov eax, [0x5080]
	LEAVE

_get_esp:
    xor rax, rax
    mov rax, rsp
    add rax, 16
    ret

_get_ebp:
    xor rax, rax
    mov rax, rbp
    ret

_get_r9:
    xor rax, rax
    mov rax, r9
    ret

_get_r11:
    xor rax, rax
    mov rax, r11
    ret

_get_r12:
    xor rax, rax
    mov rax, r12
    ret