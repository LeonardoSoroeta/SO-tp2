global _outport
global _inport
global _outportb
global _inportb
global _cli
global _sti
global _halt

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

section .text

;=========================
; _outport
;=========================
; void _outport(int,int)
;=========================
; Escribe en un puerto el
; contenido de Value
;
; Argumentos
;	- Port: di
;	- Value: si
_outport:
	ENTER
	mov rax, 0
	mov rdx, 0

	mov ax, si
	and ax, 0xff
	mov dx, di
	out dx, al

	LEAVE

;=========================
; _inport
;=========================
; void _inport(int)
;=========================
; Lee el contenido de un
; puerto
;
; Argumentos
;	- Port: di
; Salida
;	- BCD del valor leido
_inport:
	ENTER
	mov dx, di
	in al, dx
	LEAVE

;=========================
; _cli
;=========================
; Deshabilita
; interrupciones
_cli:
	cli
	ret

;=========================
; _sli
;=========================
; Habilita interrupciones
_sti:
	sti
	ret

;=========================
; _halt
;=========================
; Pone el CPU en Idle
; hasta que entre una
; interrupcion
_halt:
	hlt
	ret
