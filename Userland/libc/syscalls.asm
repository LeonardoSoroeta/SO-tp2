global _syscaller
global yield

section .text

_syscaller:
	int 80h
	ret


; void fork(char* name);
fork:
		int 0x79
		
		mov rsi, r
		mov rdx, rdi ; set name to second parameter
		mov rdi, 11  ; set syscall id
		; rdi=FORK rsi=ret addr rdx=name
		int 0x80
r:	ret

; int yield(void)
yield:
	int 0x79; backup state
	mov rdi, 0x2d ; set syscall id
	int 0x80
	int 0x79; backup state
	ret