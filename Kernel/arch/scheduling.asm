extern current_entry_point

extern drop_to_kernel
extern back_to_user

global _start_scheduling

section .text

_start_scheduling:
	; get the first process's RSP and load it
	call back_to_user
	mov rsp, rax

	; launch the first process
	call current_entry_point
	jmp rax